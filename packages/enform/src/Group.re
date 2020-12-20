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
    groupMap: Belt.HashMap.String.t(string),
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
    groupMap: Belt.HashMap.String.make(~hintSize=5),
    onChange: defaultOnChange,
  };
  // [@react.component]
  // let make = (~group, ~renderer, ~members) => {
  //   <div className=[%tw ""]> "Placeholder for Selector"->ReasonReact.string </div>;
  // };
};

type groupType =
  | /** The selector mode is for where we want to change visibility/configuration based on a field */
    Selector(
      Selector.t,
    )
  | /** This is for items like radio buttons and checkboxes, where one wants one or more options selected */
    Chooser
  | /** A basic node which only ties the members into a unit so they may be validated together
     *
     *  This is used for the root node of the form
     */
    Simple;

type t = {
  guid: string,
  groupType,
  memberIds: array(string),
  common: Common.t,
  // accessors?
};

/** Add a new member to the end of the group's members */
let addMember = (memberId, group) =>
  // Only add the id if it wasn't previously added
  memberId |> A.appendStr(~noDup=true, group.memberIds) |> kC(memberIds => ok({...group, memberIds}));

/** Remove an child from the group
 *
 * @param allowUnknown Don't throw an error if the child isn't contained in the group
 */
let removeMember = (~allowUnknown=false, group, childId) => {
  let memberIds = [|childId|] |> A.filterValues(group.memberIds);
  allowUnknown || memberIds |> A.len != (group.memberIds |> A.len)
    ? ok({...group, memberIds})
    : err(
        ~msg=format("Could not remove unknown child '%s' from group '%s'", childId, group.guid),
        Errors.NotFound,
      );
};

let newSimpleGroup = (~parentId=rootGroupId, ~memberIds=[||], guid) => {
  Common.newCommonConfig(~memberOf=[|parentId|], guid)
  |> kC(common => ok({guid, groupType: Simple, common, memberIds}));
};
let newSelectorGroup = (~parentId=rootGroupId, ~memberIds=[||], ~selectorId="", guid) =>
  Common.newCommonConfig(~memberOf=[|parentId|], guid)
  |> kC(common => ok({guid, groupType: Selector(Selector.newSelector(selectorId)), common, memberIds}));

let setSelector = (group, memberId) =>
  switch (group.groupType) {
  | Selector(conf) => ok({...group, groupType: Selector({...conf, selectorId: memberId})})
  | _ => err(~msg="Tried to set the group selector on a non-selector", Errors.BadValue)
  };

[@react.component]
let make = (~group, ~dispatch, ~renderer) => {
  let _ = dispatch;
  Js.log("Who Am I");
  Js.log(group);
  switch (group.groupType) {
  | Selector(conf) =>
    Js.log("-- Rendering the Selector");
    let selector = conf.selectorId |> renderer;
    // Render all the subgroups, with only the selected one being visible
    let groups =
      conf.groupMap
      |> Belt.HashMap.String.toArray
      |> Array.map(((optionId, subGroupId)) => {
           let members = subGroupId |> renderer;
           let subGroupId = format("SelectGroup__%s__%s", group.guid, optionId);
           <div id=subGroupId key=subGroupId> members </div>;
         })
      |> ReasonReact.array;
    Js.log(conf);
    Js.log("After");
    <div className=[%tw "form-group"] key={group.guid}>
      selector
      <div className=[%tw "col-md-3 col-xs-12 control-label"]> ReasonReact.null </div>
      <div className=[%tw "col-md-6 col-xs-12 field-group"]> groups </div>
    </div>;

  | Simple =>
    Js.log(group.memberIds);
    group.memberIds |> Array.map(renderer) |> ReasonReact.array;

  | _ => <div key={group.guid}> "Non-Select group not yet implemented"->ReasonReact.string </div>
  };
};
