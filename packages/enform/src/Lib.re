/** The main module to tie everything together
 *
 * Style wise, I'm making this the public interface so internal relationships can be maintained by the code
 * and not by the end user.  In addition, this is the standard method of avoiding side effects in functional/
 * immutable coding.
 *
 */
// open Eeyore;
open Helpers;

let newForm = Form.newForm;

// Group Management

let getGroup = (form: Form.t, groupId) => {
  HM.find(
    ~notFound=Some(format("Could not find group with id '%s' in form '%s'", groupId, form.guid)),
    form.members,
    groupId,
  )
  |> kC(group =>
       switch (group) {
       | Form.Member.Group(x) => ok(x)
       | Form.Member.Field(_) =>
         err(~msg=format("Id '%s' belongs to a field and not a group", groupId), Errors.BadValue)
       }
     );
};

let getField = (form: Form.t, fieldId) => {
  HM.find(
    ~notFound=Some(format("Could not find field with id '%s' in form '%s'", fieldId, form.guid)),
    form.members,
    fieldId,
  );
};

let addGroup = (form: Form.t, newGroup: Group.t): Result.t(Form.t) => {
  // Get the root group
  let getRoot = () => getGroup(form, "__root_group");

  // Make sure each of the members contained in the new group exist within the form
  let verifyMembers = () =>
    HM.contains(
      newGroup.memberIds,
      form.members,
      ~keysNotFoundMsg=
        format("Not all members of group '%s' exist in form '%s'. Missing: ", newGroup.guid, form.guid),
    );

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
    let updated =
      Array.fold_left(
        (acc, key) =>
          switch (Belt.Array.getIndexBy(root.memberIds, a => a == key)) {
          | Some(_) => acc
          | None => Array.concat([acc, [|key|]])
          },
        [||],
        newGroup.memberIds,
      );

    Belt.HashMap.String.set(
      form.members,
      root.guid,
      Form.Member.Group({...root, memberIds: Array.concat([updated, [|newGroup.guid|]])}),
    );
    ok(form);
  };

  //  |> kC(_ => getGroup(form, "__root_group")) |> kC(_root => {ok(form)});
  // Validate nested members exist,
  // Add the group to groups
  // add membership in group to each field remove from root if need be
  //
  verifyMembers()
  |> kC(_ => getRoot())
  |> kC(root => insertGroup(root, form))
  |> kC(((root: Group.t, form: Form.t)) => updateRootGroup(root, form));
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
