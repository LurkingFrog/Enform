/** Sub-grouping of fields
 *
 * This is for interrelated fields so we can validate on minipulate them as a whole, such as toggling the
 * address display based on the selected country.
 *
 * This is usually generated from an enumeration
 * */;

open Helpers;

/** This displays a given subform based on the currently selected choice */
module Selector = {
  type t = {
    /** The field that drives the state of the entire group */
    selector: string,
    /** Fired when the selector is changed. It takes the value of */
    onChange: string => Belt.Result.t(unit, Errors.t),
  };

  let defaultOnChange = value => {
    Js.log("Clicked selector with value: " ++ value);
    ok();
  };

  let newSelector = selector => {selector, onChange: defaultOnChange};
  // let default_onChange = (newValue) =>

  [@react.component]
  let make = (~field, ~reducer) => {
    Js.log(field);
    Js.log(reducer);
    <div className=[%tw ""]> "Placeholder for Selector"->ReasonReact.string </div>;
  };
};

type groupType =
  | /** The selector mode is for where we want to change visibility/configuration based on a field */
    Selector(
      Selector.t,
    )
  | /** This is for items like radio buttons and checkboxes, where one wants one or more options selected */
    Chooser
  | Validation;

type t = {
  guid: string,
  groupType,
  memberIds: array(string),
  // accessors?
};

let newValidationGroup = guid => ok({guid, groupType: Validation, memberIds: [||]});

[@react.component]
let make = (~conf, ~reducer, ~children) => {
  let body =
    switch (conf.groupType) {
    | Selector(field) => <Selector field reducer />
    | _ => <div> "Non-Select group not yet implemented"->ReasonReact.string </div>
    };

  <div className=[%tw "field-group"]>
    <div className=[%tw "form-group"]>
      <label className=[%tw "col-md-3 col-xs-12 control-label"]> "Select"->ReasonReact.string </label>
      body
    </div>
    {"Placeholder for Group Named" ++ conf.guid |> ReasonReact.string}
    children
  </div>;
};
