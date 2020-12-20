/** Some common shortcuts and aliases used throughout the software.
 *
 * This is mostly wrapping the Belt functions in Eeyo errors.
 *
 * These are not specific to any specific module or type defined in this package, and are meant for internal
 * use only. Possibly to be re-exorted as a separate module, since I use most of these patterns in all of my
 * code.
 *
 * TODO:
 * - Convert to ReScript if moved to a separate module
 * - Move this all into Eeyo, as the primary function of this module is to ROP enable Belt
 */;

// -- Library Constants --
/** Hard coded member Id for the root node of each form */
let rootGroupId = "__root_group";

// -- End Library Constants --

// -- Error Handling Functions --
open Eeyore;

module Result = {
  type t('a) = Belt.Result.t('a, Eeyo.t(Errors.t));
};
let ok = Eeyo.ok;
/** Return the unit Ok result. Auto-formatting seems to have trouble with this. */
let uOk = (): Result.t(unit) => ok();
let err = Eeyo.err;
let kC = Eeyo.kC;
let apply = Eeyo.apply;
let curry = Eeyo.curry;
let chain = Eeyo.chain;
let flattenExn = Eeyo.flattenExn;
let getExn = Eeyo.getExn;
let concatExn = Eeyo.concatExn;
let map = Eeyo.map;
let mapErr = Eeyo.mapErr;
let ifOk = Eeyo.map;
// -- End Error Handling Functions --

/** Re-export sprintf as format */
let format = x => Printf.sprintf(x);

/** Some internal debugging tools that shouldn't end up in the live code*/
module Debugging = {
  /** Write a checkpoint to the Javascript console and echo the value   */

  let chpt = (~msg="", value) => {
    Js.log(format("Checkpoint: %s", msg));
    Js.log(value);
    value;
  };
};

/** Convert None to a NotFound error.
 *
 *  */
let mapNone = (~msg="Received a None instead of Some(value)", ~level=Eeyo.Err, errorType: Errors.t, item) =>
  switch (item) {
  | Some(value) => Eeyo.ok(value)
  | None =>
    switch (level) {
    | Panic => Js.Exn.raiseError(msg)
    | Warn
    | Err
    | Info => Eeyo.err(~msg, ~level, errorType)
    }
  };

/** Convert an unexpected some to an error or a unit Ok if None was found
 *
 * This is generally used as a test for duplication in a list, since we want to handle an update differently
 * than an insert
 */
let mapSome = (~msg="Received a Some where None was expected", ~level=Eeyo.Err, errorType: Errors.t, item) =>
  switch (item) {
  | None => Ok()
  | Some(_) =>
    switch (level) {
    | Panic => Js.Exn.raiseError(msg)
    | Warn
    | Err
    | Info => Eeyo.err(~msg, ~level, errorType)
    }
  };

/** Insert a new value into an array
 *
 * @position Where we should add the new item. If None (the default), it will be appended to the end
*/
// FIXME: Testing for the insert many, the offest/bounds are not tested at all and the edge cases will blow up
let insert = (~position=None, values: array('a), item: 'a) => {
  Belt.Array.(
    switch (position) {
    | None => ok(concat(values, [|item|]))
    | Some(pos) =>
      ok(concatMany([|slice(~offset=0, ~len=pos - 1, values), [|item|], sliceToEnd(values, pos)|]))
    }
  );
};

/** Result functions */
module R = {
  //
  let unwrap = (result: Belt.Result.t('a, Eeyo.t('b))) =>
    switch (result) {
    | Ok(x) => x
    | Error(x) => Js.Exn.raiseError(format("Tried to unwrap an error:\n%s", x.msg))
    };

  /** Elevate a scalar result into an array with one element */
  let intoArray = result =>
    switch (result) {
    | Ok(x) => Ok([|x|])
    | Error(x) => Error(x)
    };
};

/** Belt.Option wrappers */
module O = {
  /** Alias for Belt.Option.getWithDefault */
  let getOr = (opt, default) => opt |> Belt.Option.getWithDefault(default);

  /** map a function to the value if the option contains a value, or return an error
   *
   * The default function is option('a) => Ok('a)
  */
  let mapSome =
      (~errMsg="Received a None value when Some was expected", ~errType="Errors.IsNone", ~func=?, value) =>
    switch (value, func) {
    | (Some(value), None) => ok(value)
    | (Some(value), Some(func)) => value |> func
    | (None, _) => err(~msg=errMsg, errType)
    };
};

/** Array functions */
module A = {
  /** Alias for length */
  let len = Belt.Array.length;

  /** Append a single value to a list */
  let append = (arr, value) => Array.concat([arr, [|value|]]);

  /** Append a result into an array of results, if both are Ok. Flatten the errors if not */
  let appendR = (~_groupErrorMsg=None, scalar: Belt.Result.t('a, 'b), base: Belt.Result.t(array('a), 'b)) => {
    let curried = curry(append, base);
    ok((curried |> R.unwrap)(R.unwrap(scalar)));
    // apply(~groupErrorMsg, curried, scalar);
  };

  /** Append a single value to the end of an existing array
   *
   * @param unique Raise an error if the item to be added already exists in the array
   * @param noDup Skip adding the value to the array if it already exists
  */
  let appendStr = (~unique=false, ~noDup=false, arr, value) => {
    switch (unique, noDup) {
    | (false, false) => ok(Array.concat([arr, [|value|]]))
    | (true, _) =>
      switch (Belt.Array.getIndexBy(arr, x => x == value)) {
      | Some(_) =>
        err(~msg=format("Failed to append '%s' because it was not unique", value), Errors.DuplicateKey)
      | None => ok(Array.concat([arr, [|value|]]))
      }
    | (false, true) =>
      switch (Belt.Array.getIndexBy(arr, x => x == value)) {
      | Some(_) => ok(arr)
      | None => ok(Array.concat([arr, [|value|]]))
      }
    };
  };

  /** Check whether a string is contained in an array, and return the indicies of matching values
   *
   * TODO: Move this into Algae.B.re
  */
  let hasStr = (~notFoundMsg=?, ~firstOnly=true, arr, value) => {
    let result =
      firstOnly
        ? switch (Belt.Array.getIndexBy(arr, (a: string) => a == value)) {
          | Some(i) => [|i|]
          | None => [||]
          }
        : Belt.Array.reduceWithIndex(arr, [||], (acc, key, i) => key == value ? append(acc, i) : acc);
    switch (result |> Belt.Array.length, notFoundMsg) {
    | (0, Some(msg)) => err(~msg, Errors.NotFound)
    | (0, None) =>
      err(~msg=format("The string '%s' is not a member of the array", value), Errors.NotFound)
    | _ => ok(result)
    };
  };

  /** Remove an array of keys from the array
   *
   * TODO:
   * - Make sure it works for repeat values
   */
  let filterValues = (full, filter) => {
    Array.fold_left(
      (acc, key) =>
        switch (Belt.Array.getIndexBy(filter, (a: string) => a == key)) {
        | Some(_) => acc
        | None => append(acc, key)
        },
      [||],
      full,
    );
  };
  /** Return an array of unwrapped values */
  let filterNone = () => {
    Array.fold_left(
      (acc, value) =>
        switch (value) {
        | Some(x) => append(acc, x)
        | None => acc
        },
      [||],
    );
  };

  let flattenExn = (~groupErrorMsg=None, arr) =>
    arr |> Array.to_list |> Eeyo.flattenExn(~groupErrorMsg) |> kC(arr => ok(arr |> Array.of_list));

  let flatMap = (~groupErrorMsg=None, func, arr) => Array.map(func, arr) |> flattenExn(~groupErrorMsg);
};

/** List functions */
module L = {
  /** Append a single value to the end of an existing array */
  let append = (arr, value) => List.concat([arr, [value]]);

  /** Remove an array of keys from the array
   *
   * TODO:
   * - Make sure it works for repeat values
   */
  let filterValues = (full, filter) =>
    List.fold_left(
      (acc, key) =>
        switch (Belt.List.getBy(full, (a: string) => a == key)) {
        | Some(_) => acc
        | None => append(acc, key)
        },
      [],
      filter,
    );

  /** Return an array of unwrapped values */
  let filterNone = () => {
    List.fold_left(
      (acc, value) =>
        switch (value) {
        | Some(x) => append(acc, x)
        | None => acc
        },
      [],
    );
  };
};

/** Some hashmap functions I tend to use a lot */
module HM = {
  let find = (~notFound=None, hashMap, key) => {
    Belt.HashMap.String.get(hashMap, key)
    |> mapNone(
         ~msg=
           notFound->Belt.Option.getWithDefault(
             format("Item with key '%s' was not found in the hash map", key),
           ),
         ~level=
           switch (notFound) {
           | Some(_) => Err
           | None => Panic
           },
         NotFound,
       );
  };
  let has = (~notFound=None, hashMap, key) =>
    switch (Belt.HashMap.String.get(hashMap, key)) {
    | Some(_) => uOk()
    | None =>
      err(
        ~msg=
          notFound->Belt.Option.getWithDefault(
            format("HashMap does not contain an item with key '%s'", key),
          ),
        ~level=
          switch (notFound) {
          | Some(_) => Err
          | None => Panic
          },
        Errors.NotFound,
      )
    };

  /** Uniquely insert a new item with key and raise a duplicate key error if it already exists */
  let insert = (~dupError=None, key, item, hashMap) => {
    Belt.HashMap.String.get(hashMap, key)
    |> mapSome(
         ~msg=dupError |> O.getOr(format("Item with key '%s' already exists in the hash map", key)),
         ~level=
           switch (dupError) {
           | Some(_) => Err
           | None => Panic
           },
         NotFound,
       )
    |> kC(() => {
         Belt.HashMap.String.set(hashMap, key, item);
         ok(hashMap);
       });
  };

  /** Update a HashMap item in place */
  let update = (~notFound=None, key, func, hashMap) => {
    find(~notFound, hashMap, key)
    |> kC(func)
    |> kC(item => {
         Belt.HashMap.String.set(hashMap, key, item);
         ok(hashMap);
       });
  };

  /** Ensure a list of keys is exists within a hashmap */
  let getValues = (~keysNotFoundMsg, ~ignoreMissing=false, keys, hashMap) =>
    keys
    |> Array.fold_left(
         (acc, key) => {
           let value = Belt.HashMap.String.get(hashMap, key);
           switch (ignoreMissing, value) {
           | (_, Some(x)) => A.append(acc, ok(x))
           | (true, None) => acc
           | (false, None) => A.append(acc, err(~msg=format("Key '%s' not found", key), Errors.NotFound))
           };
         },
         [||],
       )
    |> A.flattenExn(~groupErrorMsg=Some(keysNotFoundMsg));
};

/** Get the last value in an array
 *
 * This returns an error if the array is empty
*/
let last = arr => {
  (arr |> Belt.Array.length)
  - 1
  |> Belt.Array.get(arr)
  |> mapNone(~msg="Cannot get the last item of an empty array", OutOfRange);
};

/** Add back button functionality since it doesn't seem to be in the router
 *
*/

let back = () => {
  Webapi.Dom.(History.(back(history)));
};

/** A way to add extra props that are used by external javascript modules */
module WithProps = {
  [@react.component]
  let make = (~props, ~children) => React.cloneElement(children, props);
};
