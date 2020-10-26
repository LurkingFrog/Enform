/** Sub-grouping of fields
 *
 * This is for interrelated fields so we can validate on minipulate them as a whole, such as toggling the
 * address display based on the selected country.
 *
 * This is usually generated from an enumeration
 * */;

open Helpers;

/** This is an option based  */
module Selector = {
  type t = {
    /** The field that drives the state of the entire group */
    selector: string,
    managedFields: list(string),
    /** Fired when the selector is changed. It takes the value of */
    onChange: string => Belt.Result.t(unit, Errors.t),
  };

  let defaultOnChange = value => {
    Js.log("Clicked selector with value: " ++ value);
    ok();
  };

  let newSelector = selector => {selector, managedFields: [], onChange: defaultOnChange};
  // let default_onChange = (newValue) =>
};

type groupType =
  | /** The selector mode is for where we want to change visibility/configuration based on a field */
    Selector(
      Selector.t,
    )
  | /** This is for items like checkboxes, where one wants one or more options selected */
    Chooser
  | Validation;

type t = {
  guid: string,
  groupType,
  fieldGuids: list(string),
};

let newGroup = (~fieldGuids=[], ~groupType=Validation, guid) => {guid, groupType, fieldGuids};
