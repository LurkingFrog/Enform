/** The form backend, with full configuration */
open Helpers;

module Form = {
  module Config = {
    type t = {validator: option(unit => unit)};

    let newConfig = (~validator=None, ()): t => {validator: validator};
  };

  type t = {
    config: Config.t,
    className: option(string),
    header: option(string),
  };

  let newForm = () => {config: Config.newConfig(), className: None, header: None};
  let render = (~_form: t) => {
    <div> "I'm the rendered form"->ReasonReact.string </div>;
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
  config: Form.t,
  /** Current overall status of the form */
  formState,
  /** The individual fields */
  fields: Belt.HashMap.String.t(Field.t),
  /** A shortcut for verifying the tab order of the fields*/
  // FIXME: This isn't doing anything useful yet.
  _tabOrder: array(string),
};

let newEnformer = () => {
  guid: Uuid.make(),
  config: Form.newForm(),
  formState: Empty,
  fields: Belt.HashMap.String.make(~hintSize=5),
  _tabOrder: [||],
};

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

/**
 *
 */

let _renderField = (~form: t, ~fieldName: string) => {
  Js.log("Rendering Enform field " ++ fieldName);
  Js.log(form);
  <div> "Placeholder for rendering a field"->ReasonReact.string </div>;
};
