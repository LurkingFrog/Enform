/** The form backend, with full configuration */
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
    | Group(Group.t);
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
};

let newForm = (): Result.t(t) => {
  let form = {
    guid: Uuid.make(),
    config: Config.newConfig(),
    formState: Empty,
    members:
      Belt.HashMap.String.fromArray([|
        ("__root_group", Member.Group(Group.newValidationGroup("__root_group") |> getExn)),
      |]),
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

 /** Add a single field */
 let addField = (field: Field.t, enformer: t) => {
   enformer.fields
   |> HM.insert(
        ~dupError=Some(sprintf("Item with key '%s' already exists in the hash map", field.guid)),
        field.guid,
        field,
      )
   |> kC(fields => ok({...enformer, fields, _tabOrder: field.guid |> insert(enformer._tabOrder) |> getExn}));
 };

 let getField = (guid: string, enformer: t) => {
   HM.find(
     ~notFound=Some(sprintf("Field with key '%s' was not registered with the current enformer", guid)),
     enformer.fields,
     guid,
   );
 };

 let setField = (field: Field.t, enformer: t) => {
   Belt.HashMap.String.set(enformer.fields, field.guid, field);
   ok(enformer);
 };

 /** Add a set of fields to an existing form in ordered fashion */
 let addFields = (enformer: t) => {
   Array.fold_left((acc, field) => acc |> kC(addField(field)), ok(enformer));
 };

 let addFieldsToGroup = (fields: array(string), guid: string, enformer: t) => {
   // Verify all the fields exist
   fields
   |> Array.map(field => getField(field, enformer))
   |> Array.to_list
   |> Eeyo.flattenExn
   |> kC(_ => enformer |> getField(guid))
   |> kC((group: Field.t) =>
        switch (group.fieldType) {
        | Group(conf) => {...group, fieldType: Group(conf)}->setField(enformer)
        | _ => err(~msg=sprintf("Field '%s' is not a group", guid), Errors.InvalidEnum)
        }
      );
 };

 /** A quick renderer of an entire form
  *
  * This returns a full panel with the configured layout.
 */
 [@react.component]
 let make = (~form, ~reducer) => {
   // Render each field individually by tab order, hiding everything in groups.
   let fields = form._tabOrder |> Array.map(fieldId => <Field form fieldId reducer />);

   // For each __root item, render it
   <div className=[%tw "form-material"]> {fields |> ReasonReact.array} </div>;
 };
  */
