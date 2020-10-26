/** Some common shortcuts and aliases used throughout the software */;

// To fix this random compilation issue, I included this module
// open Eeyore

/** Re-export Eeyo's Kleisli Composition */
let ok = Eeyo.ok;
let err = Eeyo.err;
let kC = Eeyo.kC;
let getExn = Eeyo.getExn;

/** Re-export sprintf */
let sprintf = Printf.sprintf;

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

/** Some hashmap functions I tend to use a lot */
module HM = {
  let find = (~notFound=None, hashMap, key) => {
    Belt.HashMap.String.get(hashMap, key)
    |> mapNone(
         ~msg=
           notFound->Belt.Option.getWithDefault(
             sprintf("Item with key '%s' was not found in the hash map", key),
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
             sprintf("Item with key '%s' already exists in the hash map", key),
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
  let update = (~notFound=None, hashMap, key, func) => {
    find(~notFound, hashMap, key)
    |> kC(func)
    |> kC(item => {
         Belt.HashMap.String.set(hashMap, key, item);
         ok(hashMap);
       });
  };
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
