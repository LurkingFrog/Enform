/** Sub-grouping of fields
 *
 * This is for interrelated fields so we can validate on minipulate them as a whole, such as toggling the
 * address display based on the selected country.
 *
 * This is usually generated from an enumeration
 *
 * TODO:
 * Add a composite selector. Something like a group of checkboxes should be able to select the group/
 * */;

open Helpers;

/** This displays a given subform based on the currently selected choice */
module Selector = {
  type t = {
    /** The field that drives the state of the entire group */
    selectorId: string,
    /** Group to be displayed for each value of selector */
    groups: Belt.HashMap.String.t(string),
    //
    // ----  Events
    /** Fired when the selector is changed. It takes the value of */
    onChange: string => Belt.Result.t(unit, Errors.t),
  };

  let defaultOnChange = value => {
    Js.log("Clicked selector with value: " ++ value);
    ok();
  };

  let newSelector = selectorId => {
    selectorId,
    groups: Belt.HashMap.String.make(~hintSize=5),
    onChange: defaultOnChange,
  };

  [@react.component]
  let make = (~field, ~dispatch) => {
    Js.log(field);
    Js.log(dispatch);
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
  | /** A basic node which only ties the members into a unit so they may be validated together */
    Simple;

type t = {
  guid: string,
  groupType,
  parentId: string,
  memberIds: array(string),
  // accessors?
};

let newSimpleGroup = (~parentId=rootGroupId, guid) =>
  ok({guid, groupType: Simple, parentId, memberIds: [||]});

let newSelectorGroup = (~parentId=rootGroupId, ~memberIds=[||], ~selectorId="", guid) =>
  ok({guid, groupType: Selector(Selector.newSelector(selectorId)), parentId, memberIds});

let setSelector = (group, memberId) =>
  switch (group.groupType) {
  | Selector(conf) => ok({...group, groupType: Selector({...conf, selectorId: memberId})})
  | _ => err(~msg="Tried to set the group selector on a non-selector", Errors.BadValue)
  };

[@react.component]
let make = (~group, ~dispatch, ~children) => {
  let body =
    switch (group.groupType) {
    | Selector(field) => <Selector field dispatch />
    | _ => <div> "Non-Select group not yet implemented"->ReasonReact.string </div>
    };

  <div className=[%tw "field-group"]>
    <div className=[%tw "form-group"]>
      <label className=[%tw "col-md-3 col-xs-12 control-label"]> "Select"->ReasonReact.string </label>
      body
    </div>
    {"Placeholder for Group Named" ++ group.guid |> ReasonReact.string}
    children
  </div>;
};
