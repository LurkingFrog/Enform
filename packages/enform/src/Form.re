/** The form backend, with full configuration
 *
 * TODO:
 * - Tab Ordering
*/
open Helpers;

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
    | Group(Group.t)
    | /** Variable number of clones of the same type of member - Like having different phone-numbers */
      List(
        string,
        array(t),
      );

  let getId = member =>
    switch (member) {
    | Field(x) => x.guid
    | Group(x) => x.guid
    | List(x, _) => x
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

/** Add a single field */
let addField = (field: Field.t, form: t) => {
  form.members
  |> HM.insert(
       ~dupError=Some(format("Item with key '%s' already exists in the hash map", field.guid)),
       field.guid,
       Member.Field(field),
     )
  |> kC(members => ok({...form, members, _tabOrder: field.guid |> insert(form._tabOrder) |> getExn}));
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

// let addFieldsToGroup = (fields: array(string), guid: string, enformer: t) => {
//   // Verify all the fields exist
//   fields
//   |> Array.map(field => getField(field, enformer))
//   |> Array.to_list
//   |> flattenExn(~groupErrorMsg="Failed to all fields to the group:")
//   |> kC(_ => enformer |> getField(guid))
//   |> kC((group: Field.t) =>
//        switch (group.fieldType) {
//        | Member.Group(conf) => {...group, fieldType: Group(conf)}->setField(enformer)
//        | _ => err(~msg=sprintf("Field '%s' is not a group", guid), Errors.InvalidEnum)
//        }
//      );
// };

let getMember = (form: t, memberId) => {
  HM.find(
    ~notFound=Some(format("Could not find member with id '%s' in form '%s'", memberId, form.guid)),
    form.members,
    memberId,
  );
};

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

let getRoot = form => {
  getGroup(form, Helpers.rootGroupId);
};

// Make sure each of the members contained in the new group exist within the form
let verifyMembers = (form, memberIds) =>
  HM.getValues(
    memberIds,
    form.members,
    ~keysNotFoundMsg=format("Not all members exist in form '%s'. Missing: ", form.guid),
  );

let rec renderMember = (form, dispatch, memberId) => {
  let member =
    switch (memberId |> getMember(form) |> getExn) {
    | Member.Group(group) =>
      let children = group.memberIds |> Array.map(renderMember(form, dispatch)) |> ReasonReact.array;
      <Group group dispatch> children </Group>;
    | Member.Field(field) => <Field field dispatch />
    | Member.List(_, members) =>
      members
      |> Array.map(member => member |> Member.getId |> renderMember(form, dispatch))
      |> ReasonReact.array
    };

  <div className=[%tw ""] key=memberId> member </div>;
};

/** A quick renderer of an entire form
  *
  * This returns a full panel with the configured layout.
 */
[@react.component]
let make = (~form, ~dispatch) => {
  let root_group = getGroup(form, rootGroupId) |> getExn;
  Js.log("\n\nRendering the form");
  Js.log(root_group);

  // Render each field individually by tab order, hiding everything in groups.
  let fields = root_group.memberIds |> Array.map(renderMember(form, dispatch));

  // For each __root item, render it
  <div className=[%tw "form-material"]> {fields |> ReasonReact.array} </div>;
};
