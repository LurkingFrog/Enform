/** The form backend, with full configuration
 *
 * TODO:
 * - Tab Ordering
*/
open Helpers;
open Debugging;

module Config = {
  type t = {
    validator: option(unit => unit),
    className: option(string),
    header: option(string),
  };

  let newConfig = (~validator=None, ()): t => {validator, className: None, header: None};
};

module Member = {
  type t =
    | Field(Field.t)
    | Group(Group.t);
  // | /** Variable number of clones of the same type of member - Like having different phone-numbers */
  //   List(
  //     string,
  //     array(t),
  //   );

  let getId = member =>
    switch (member) {
    | Field(x) => x.guid
    | Group(x) => x.guid
    // | List(x, _) => x
    };

  /** Get the ids of groups to which the member belongs */
  let rec getMemberOf = member => {
    switch (member) {
    | Field(x) => x.common.memberOf
    | Group(x) => x.common.memberOf
    };
  };

  let setMemberOf = (member, membershipIds) => {
    switch (member) {
    | Field(x) => Field({
                    ...x,
                    common: {
                      ...x.common,
                      memberOf: membershipIds,
                    },
                  })
    | Group(x) => Group({
                    ...x,
                    common: {
                      ...x.common,
                      memberOf: membershipIds,
                    },
                  })
    };
  };

  /** Uniquely append a child id to the given group type memberId
   *
   * From level validation should be done before this call.
   */
  let addChild = (parent, childId) => {
    switch (parent) {
    | Field(x) =>
      err(
        ~msg=format("Failed to add child '%s' because '%s' is not a group", childId, x.guid),
        Errors.BadValue,
      )
    | Group(x) => Group.addMember(childId, x) |> kC(x => ok(Group(x)))
    };
  };

  let removeChild = (~allowUnknown=true, member, childId) => {
    switch (member) {
    | Field(x) =>
      err(
        ~msg=
          format(
            "Failed to remove child '%s' because '%s' is a field type and has no children to remove",
            x.guid,
            childId,
          ),
        Errors.BadValue,
      )
    | Group(x) => Group.removeMember(~allowUnknown, x, childId) |> kC(x => ok(Group(x)))
    };
  };
};

/** Keep track of the form as a whole */
type formState =
  | /** Only filled with the default values */
    Empty
  | /** Some fields filled out, but not ready for submission */
    Partial
  | /** The minimum required fields are filled in and validate as a group */
    Validated
  | /** The submit button has been clicked, and waiting for a response */
    Submitted
  | /** The submitted form either failed validation or submission returned an error */
    Error
  | /** The server has acknowleged the request */
    Acknowleged
  | /** Received the result from the server */
    Done;

type t = {
  /** A unique identifier for the instance of Enformer */
  guid: string,
  /** Overall form configuration */
  config: Config.t,
  /** Current overall status of the form */
  formState,
  /** The individual fields */
  members: Belt.HashMap.String.t(Member.t),
  /** The order the fields should be tabed */
  _tabOrder: array(string),
};

let newForm = (~guid=?, ()): Result.t(t) => {
  let form = {
    guid: guid |> O.getOr(Uuid.make()),
    config: Config.newConfig(),
    formState: Empty,
    members:
      Belt.HashMap.String.fromArray([|
        (rootGroupId, Member.Group(Group.newSimpleGroup(rootGroupId) |> getExn)),
      |]),
    _tabOrder: [||],
  };
  ok(form);
};

/*
   let updateTabOrder = (order: array(string), enformer: t) => {
     // Test Length, error if not all fields are in the order
     // Loop, Error if an order field isn't registered
     ok({
       ...enformer,
       _tabOrder: order,
     });
   };
 */

let getGroup = (form: t, groupId) => {
  HM.find(
    ~notFound=Some(format("Could not find group with id '%s' in form '%s'", groupId, form.guid)),
    form.members,
    groupId,
  )
  |> kC(group =>
       switch (group) {
       | Member.Group(x) => ok(x)
       | _ => err(~msg=format("Id '%s' belongs to a field and not a group", groupId), Errors.BadValue)
       }
     );
};

/** Retrieve a member from the form by its guid */
let getMember = (form: t, memberId) => {
  HM.find(
    ~notFound=Some(format("Could not find member with id '%s' in form '%s'", memberId, form.guid)),
    form.members,
    memberId,
  );
};

/** Upsert a member in a given form */
let setMember = (form: t, member: Member.t) => {
  Belt.HashMap.String.set(form.members, member |> Member.getId, member);
  ok(form);
};

let getMember = (form: t, memberId) => {
  HM.find(
    ~notFound=Some(format("Could not find field with id '%s' in form '%s'", memberId, form.guid)),
    form.members,
    memberId,
  );
};

/** Move the member from its existing group (root by default) into a sub-group
 *
 * @param form The form which the member and target are registered with
 * @param memberId The guid of the member to be added
 * @param targetId The guid of the group to add the member to
 * @param allowFromRootOnly Throw an error if the member is a member of any groups other than the form's root group. Defaults to true.
 * @param removeFromOtherGroups This makes sure that the target is the only group thet item is a member of, clearing the rest. Defaults to true.
 */
let addMemberToGroup = (~removeFromOtherGroups=true, ~allowFromRootOnly=true, form: t, memberId, targetId) => {
  // Js.log(
  //   format(
  //     "Moving member '%s' to '%s'. Remove from other groups: %b, allow from root only: %b ",
  //     memberId,
  //     targetId,
  //     removeFromOtherGroups,
  //     allowFromRootOnly,
  //   ),
  // );
  let member = memberId |> getMember(form);
  let target = targetId |> getMember(form);

  let removeMemberFromParents = parents => {
    parents
    |> Array.fold_left(
         (acc, parentId) => {
           parentId == targetId
             ? acc
             : A.append(
                 acc,
                 parentId
                 |> getMember(form)
                 |> kC(parent => Member.removeChild(~allowUnknown=true, parent, memberId)),
               )
         },
         [||],
       )
    |> A.flattenExn(
         ~groupErrorMsg=Some(format("Failed to update the existing parents of member '%s':", memberId)),
       );
  };

  [|member, target|]
  |> A.flattenExn(
       ~groupErrorMsg=Some(format("Could not add member '%s' to group '%s':", memberId, targetId)),
     )
  |> kC(_ => {
       let memberOf = member |> R.unwrap |> Member.getMemberOf;
       switch (allowFromRootOnly) {
       | false => ok(memberOf)
       | true => rootGroupId |> A.hasStr(memberOf) |> kC(_ => ok(memberOf))
       };
     })
  |> kC(memberOf => {
       // Member - Change MemberOf (add/replace target)
       (removeFromOtherGroups ? removeMemberFromParents(memberOf) : ok([||]))
       |> A.appendR(
            removeFromOtherGroups
              ? ok(Member.setMemberOf(member |> R.unwrap, [|targetId|]))
              : targetId
                |> A.appendStr(member |> R.unwrap |> Member.getMemberOf)
                |> kC((membership: array(string)) =>
                     ok(Member.setMemberOf(member |> R.unwrap, membership))
                   ),
          )
       |> A.appendR(R.unwrap(target)->Member.addChild(memberId))
     })
  |> kC(updatedMembers
       // Add the updated fields back to the form
       =>
         updatedMembers
         |> Array.fold_left((acc, member) => acc |> kC(form => setMember(form, member)), ok(form))
       );
};

/** Shortcut to add multiple members to the same group */
let addMembersToGroup = (~removeFromOtherGroups=true, ~allowFromRootOnly=true, memberIds, targetId, form) => {
  memberIds
  |> Array.fold_left(
       (acc, memberId) =>
         acc
         |> kC(form =>
              addMemberToGroup(~removeFromOtherGroups, ~allowFromRootOnly, form, memberId, targetId)
            ),
       ok(form),
     );
};

/** Add a manually constructed group to the form
 *
 * This validates and updates the membership based on the internal keys
*/
let addGroup = (form: t, group: Group.t) => {
  // Verify the parent members exist in the form and add the new group as a child to each
  let parents =
    group.common.memberOf
    |> Array.map(getGroup(form))
    |> A.flattenExn(
         ~groupErrorMsg=
           Some(format("The new group '%s' contains parents that don't exist in the form:", group.guid)),
       )
    |> kC(parents =>
         ok(
           parents
           |> Array.map((parent: Group.t) =>
                {...parent, memberIds: A.appendStr(~noDup=true, parent.memberIds, group.guid) |> getExn}
              ),
         )
       );

  // Verify each of the new group's children exist in the form
  let members =
    group.memberIds
    |> Array.map(getMember(form))
    |> A.flattenExn(
         ~groupErrorMsg=
           Some(
             format("The new group '%s' contains members that don't yet exist in the form:", group.guid),
           ),
       );

  parents
  |> concatExn(members)
  // Insert the new group into the form
  |> kC(_ =>
       HM.insert(
         ~dupError=
           Some(format("Group with key '%s' already exists in the form %s", group.guid, form.guid)),
         group.guid,
         Member.Group(group),
         form.members,
       )
     )
  |> kC(members => ok({...form, members, _tabOrder: group.guid |> insert(form._tabOrder) |> getExn}))
  |> kC(form => {
       // Add the new group to each parent as a child
       parents
       |> R.unwrap
       |> Array.fold_left(
            (acc, parent: Group.t) =>
              acc
              |> kC(form =>
                   addMemberToGroup(
                     ~removeFromOtherGroups=true,
                     ~allowFromRootOnly=false,
                     form,
                     group.guid,
                     parent.guid,
                   )
                 ),
            ok(form),
          )
     })
  |> kC(form => {
       // Add move each member to the group as a child
       members
       |> R.unwrap
       |> Array.fold_left(
            (acc, child: Member.t) =>
              acc
              |> kC(form =>
                   addMemberToGroup(
                     ~removeFromOtherGroups=true,
                     ~allowFromRootOnly=true,
                     form,
                     child |> Member.getId,
                     group.guid,
                   )
                 ),
            ok(form),
          )
     });
  // validate keys
  // add new group to form
  // add the new group to each parent's members
  //
};

let getRoot = form => {
  getGroup(form, Helpers.rootGroupId);
};

/** Add a single field */
let addField = (field: Field.t, form: t) => {
  // Update each of the groups the field is a member of
  let groups =
    (
      switch (field.common.memberOf) {
      | [||] => [|rootGroupId|]
      | x => x
      }
    )
    |> A.flatMap(groupId => getGroup(form, groupId) |> kC(group => Group.addMember(field.guid, group)));

  uOk()
  |> concatExn(groups)
  |> kC(_ =>
       HM.insert(
         ~dupError=Some(format("Item with key '%s' already exists in the hash map", field.guid)),
         field.guid,
         Member.Field(field),
         form.members,
       )
     )
  |> kC(members =>
       {
         groups
         |> getExn
         |> Array.iter((group: Group.t) =>
              Belt.HashMap.String.set(members, group.guid, Member.Group(group))
            );
         ok(members);
       }
       |> kC(members => ok({...form, members, _tabOrder: field.guid |> insert(form._tabOrder) |> getExn}))
     );
};

let _getField = (guid: string, form: t) => {
  HM.find(
    ~notFound=Some(format("Field with key '%s' was not registered with the current form", guid)),
    form.members,
    guid,
  );
};

let setField = (field: Field.t, form: t) => {
  Belt.HashMap.String.set(form.members, field.guid, Member.Field(field));
  ok(form);
};

/** Retrieve a field by ID and unwrap the from the Member enum */
let getField = (form: t, fieldId) => {
  HM.find(
    ~notFound=Some(format("Could not find field with id '%s' in form '%s'", fieldId, form.guid)),
    form.members,
    fieldId,
  )
  |> kC(group =>
       switch (group) {
       | Member.Field(x) => ok(x)
       | _ => err(~msg=format("Id '%s' belongs to a group and not a field", fieldId), Errors.BadValue)
       }
     );
};

// Make sure each of the members contained in the new group exist within the form
let verifyMembers = (form, memberIds) =>
  HM.getValues(
    memberIds,
    form.members,
    ~keysNotFoundMsg=format("Not all members exist in form '%s'. Missing: ", form.guid),
  );

let rec renderMember = (form, dispatch, memberId) => {
  switch (memberId |> getMember(form) |> getExn) {
  | Member.Group(group) =>
    Js.log(format("Rendering Group Member: %s", memberId));
    <Group group dispatch renderer={renderMember(form, dispatch)} key={group.guid} />;
  | Member.Field(field) =>
    Js.log(format("Rendering Field Member: %s", memberId));
    <Field field dispatch key={field.guid} />;
  // | Member.List(_, members) =>
  //   members
  //   |> Array.map(member => member |> Member.getId |> renderMember(form, dispatch))
  //   |> ReasonReact.array
  };
};

/** Bind a simple group Id to be displayed when the option Id is selected
 *
 * By default, the optionId is the same guid as the subgroup Id
 * THINK: This should be in the group level, but no validation can happen deeper than the form level.
 *        Curry the form into the group functions?
 *
 * @ param subgroupId The guid of th sub-group that should be displayed when its option is selected
 * @ param selectorGroupId The value (not display) of the option that should trigger given subgroup to appear
 * @ param form The form that manages both the group and subgroup
*/
let addSelectorMapping = (~optionId=?, subgroupId, selectorGroupId, form) => {
  selectorGroupId
  |> getGroup(form)
  |> kC((group: Group.t) =>
       switch (group.groupType) {
       | Selector(groupConf) =>
         // make sure the subgroup is in the member Ids
         subgroupId
         |> A.hasStr(
              ~notFoundMsg=
                format("subgroup '%s' is not a member of selector group '%s'", subgroupId, group.guid),
              group.memberIds,
            )
         |> kC(_ => subgroupId |> getMember(form))
         // TODO: Change this to getMember so we can use simple groups as well as a select box
         |> kC(_ => groupConf.selectorId |> getField(form))
         |> kC((selector: Field.t) =>
              switch (selector.fieldType) {
              | Select(selectConf) =>
                optionId
                |> O.getOr(subgroupId)
                |> Select.getOption(selectConf)
                |> kC((opt: Select.selectOption) => HM.insert(opt.value, subgroupId, groupConf.groupMap))
                |> kC(groupMap =>
                     ok(Member.Group({...group, groupType: Selector({...groupConf, groupMap})}))
                   )
                |> kC(setMember(form))
              | _ =>
                err(
                  ~msg=format("Group '%s' does not have a Select field as a selector", group.guid),
                  Errors.BadValue,
                )
              }
            )

       | _ => err(~msg="Tried to add an option mapping on a non-selector group", Errors.BadValue)
       }
     );
};

/** A quick renderer of an entire form
  *
  * This returns a full panel with the configured layout.
 */
[@react.component]
let make = (~form, ~dispatch) => {
  Js.log("Rendering the form");
  Js.log(form);
  <div className=[%tw "form-horizontal"]> {renderMember(form, dispatch, rootGroupId)} </div>;
};
