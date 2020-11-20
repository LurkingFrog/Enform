/** Some common shortcuts and aliases used throughout the software
 *
 * These are not specific to any specific module or type defined in this package, and are meant for internal
 * use only. Possibly to be re-exorted as a separate module, since I use most of these patterns in all of my
 * code.
 *
 * TODO:
 * - Convert to ReScript if moved to a separate module
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
let chain = Eeyo.chain;
let flattenExn = Eeyo.flattenExn;
let getExn = Eeyo.getExn;
let concatExn = Eeyo.concatExn;
let mapErr = Eeyo.mapErr;
// -- End Error Handling Functions --

/** Re-export sprintf as format */
let format = x => Printf.sprintf(x);

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
};

/** Array functions */
module A = {
  /** Append a single value to the end of an existing array */
  let append = (arr, value) => Array.concat([arr, [|value|]]);

  /** Remove an array of keys from the array
   *
   * TODO:
   * - Make sure it works for repeat values
   */
  let filterValues = (full, filter) =>
    Array.fold_left(
      (acc, key) =>
        switch (Belt.Array.getIndexBy(full, (a: string) => a == key)) {
        | Some(_) => acc
        | None => append(acc, key)
        },
      [||],
      filter,
    );

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

  /** Uniquely insert a new item with key and raise a duplicate key error if it already exists */
  let insert = (~dupError=None, key, item, hashMap) => {
    Belt.HashMap.String.get(hashMap, key)
    |> mapSome(
         ~msg=
           dupError->Belt.Option.getWithDefault(
             format("Item with key '%s' already exists in the hash map", key),
           ),
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

/** Belt.Option wrappers */
module O = {
  /** Alias for Belt.Option.getWithDefault */
  let getOr = (opt, default) => opt |> Belt.Option.getWithDefault(default);
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
