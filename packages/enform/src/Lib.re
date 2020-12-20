/** The main module to tie everything together
 *
 * Functions that add an additional layer of validation between the user and the internals of this library.
 *
 * Style wise, I'm making this the public interface so internal relationships can be maintained by the code
 * and not by the end user.  In addition, this is the standard method of avoiding side effects in functional/
 * immutable coding.
 *
 */
open Helpers;

let newForm = Form.newForm;

let newConfig = Common.newCommonConfig;

// Field Management
/** Add a set of fields to an existing form in ordered fashion */
let addFields = (form: Form.t) => {
  Array.fold_left((acc, field) => acc |> kC(Form.addField(field)), ok(form));
};

let upsertMembers = (form: Form.t) => {
  Array.fold_left(
    (acc, field: Field.t) =>
      acc
      |> kC((acc: Form.t) => {
           Belt.HashMap.String.set(acc.members, field.guid, Form.Member.Field(field));
           ok(acc);
         }),
    ok(form),
  );
};

// Group Management
let getGroup = Form.getGroup;

let addGroup = (form: Form.t, newGroup: Group.t): Result.t(Form.t) => {
  // Get the root group
  let getRoot = () => getGroup(form, rootGroupId);

  // Add the new group to the form members
  let insertGroup = (root: Group.t, form: Form.t) => {
    HM.insert(
      ~dupError=
        Some(
          format(
            "Could not add group '%s' to form '%s' because it already exists",
            newGroup.guid,
            form.guid,
          ),
        ),
      newGroup.guid,
      Form.Member.Group(newGroup),
      form.members,
    )
    |> kC(formMembers => ok((root, {...form, members: formMembers})));
  };

  // Update the root group membership
  let updateRootGroup = (root: Group.t, form: Form.t) => {
    // Filter out the members included in the group from the root
    let filtered = A.filterValues(root.memberIds, newGroup.memberIds);
    Belt.HashMap.String.set(
      form.members,
      root.guid,
      Form.Member.Group({...root, memberIds: A.append(filtered, newGroup.guid)}),
    );
    ok(form);
  };

  // let updateMembership = ();

  // Get the root
  // get and update each member with the group as a parent
  let root = getGroup(form, rootGroupId);
  // let members = root |> kC();

  // getMembers
  Form.verifyMembers(form, newGroup.memberIds)
  |> kC(_ => getRoot())
  |> kC(root => insertGroup(root, form))
  |> kC(((root: Group.t, form: Form.t)) => updateRootGroup(root, form));
};

/** Create and add a new Selector group to a form
 *
 * NOTE: This API is assuming that we are creating the whole form in one spot, so we pass in full members
 *       instead of guids.
*/
module AddSelectorGroup = {
  /** The variables used in the kleisli chain */
  type params = {
    form: Form.t,
    root: Group.t,
    members: array(Form.Member.t),
    parent: Form.Member.t,
    newGroup: Group.t,
  };

  let getForm = params => ok(params.form);

  let members = (~selector, ~fields, ~subGroups) =>
    ok(
      Belt.Array.concatMany([|
        [|Form.Member.Field(selector)|],
        fields->Belt.Option.getWithDefault([||]) |> Array.map(x => Form.Member.Field(x)),
        subGroups->Belt.Option.getWithDefault([||]) |> Array.map(x => Form.Member.Group(x)),
      |]),
    );

  let getParent = (form, parentId) => {
    switch (parentId |> Form.getMember(form)) {
    | Ok(Form.Member.Field(_)) =>
      err(~msg="A member parent must be a Group or a List, not a Field", Errors.BadValue)
    | Ok(x) => ok(x)
    | Error(_) =>
      err(
        ~msg=
          format(
            "Could not find a member in form '%s' with id '%s' to act as the parent of the new select group",
            form.guid,
            parentId,
          ),
        Errors.NotFound,
      )
    };
  };

  let makeGroup = (~parent, ~selector, ~members, guid) => {
    Group.newSelectorGroup(
      ~parentId=parent |> Form.Member.getId,
      ~memberIds=members |> Array.map(Form.Member.getId),
      ~selectorId=selector |> Form.Member.getId,
      guid,
    );
  };

  // TODO: Convert this into an Eeyore derive all
  /** Validate the paramaters passed in and format them into something easier for the internal functions to pass around */
  let initParams = (~parentId, ~form, ~guid, ~selectorId) => {
    let root = Form.getRoot(form);
    let parent = parentId |> getParent(form);
    let selector = selectorId |> Form.getMember(form);

    uOk()
    |> concatExn(root)
    |> concatExn(parent, ~header="We found the following problems adding the selector group:")
    |> kC(_ => {
         let root = R.unwrap(root);
         let parent = R.unwrap(parent);
         let selector = R.unwrap(selector);
         let members = [|selector|];
         let newGroup = makeGroup(~parent, ~selector, ~members, guid);

         newGroup
         |> kC(Form.addGroup(form))
         |> kC((form: Form.t) => {ok({form, root, members, parent, newGroup: newGroup |> R.unwrap})});
       });
  };

  // Remove mentions of the root group from the members
  let buildGroupMap = (groupMap: array((string, array(string))), params) => {
    groupMap
    |> Array.fold_left(
         (acc, (groupName, groupMemberIds)) => {
           acc
           |> kC(params =>
                Group.newSimpleGroup(~parentId=params.newGroup.guid, groupName)
                |> kC(Form.addGroup(params.form))
                |> kC(Form.addMembersToGroup(groupMemberIds, groupName))
                |> kC(Form.addSelectorMapping(groupName, params.newGroup.guid))
                |> kC(form => ok({...params, form}))
              )
         },
         ok(params),
       );
  };
};

let addSelectorGroup = (~groupMap=[||], ~parentId=rootGroupId, ~selectorId, guid, form) => {
  AddSelectorGroup.(
    initParams(~parentId, ~form, ~guid, ~selectorId) |> kC(buildGroupMap(groupMap)) |> kC(getForm)
  );
};

// // Field Management
// let addField = (field: Field.t, form: Form.t) => {
//   // Validate the field itself
//   // Add field to member groups (if any)
//   (
//     switch (field.common.memberOf |> Array.length) {
//     | 0 => Field.addGroup(field, "__root")
//     | _ => ok(field)
//     }
//   )
//   |> kC(field => ok((field, form.groups)))
//   // Insert into fields
//   |> kC((field, groups) =>
//        HM.insert(
//          ~dupError=Some(sprintf("Item with key '%s' already exists in the hash map", field.guid)),
//          field.guid,
//          field,
//          form.fields,
//        )
//        |> kC(fields => ok(fields, groups))
//      )
//   |> kC((fields, groups) =>
//        ok({...form, fields, _tabOrder: field.guid |> insert(enformer._tabOrder) |> getExn})
//      );
// };

// Create Individual Fields
/** Convert the result of an individual field to a Form.Member.Field */
let asFieldMember = (newField: Result.t(Field.t)) => newField |> kC(x => ok(Form.Member.Field(x)));

let newTextInput = Field.newTextInput;

let newSelectInput = Field.newSelectInput;
