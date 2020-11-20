/** Field managers  */
open Helpers;

/** A generic wrapper for the individual fields
 *
 * A group is a subset of fields that work together in common events like validation or visibility.
 *
 * An example is if we want to break a large set of forms into sections. Each section can be validated and
 * saved individually, even though the form as a whole is not valid until complete.
  */
type fieldType =
  | Button(Button.t)
  | Select(Select.t)
  | Text(Text.t)
and t = {
  guid: string,
  fieldType,
  common: Common.t,
};

[@react.component]
let make = (~field, ~dispatch) => {
  Js.log("Rendering Enform field ... " ++ field.guid);
  Js.log(field);
  Js.log(dispatch);
  // Get the field
  // switch (Form.getField(fieldId, form)) {
  // | Ok(field) =>
  //   Js.log(field);
  //   switch (field.fieldType) {
  //   | Group(conf) =>
  //     let children = conf.fieldGuids |> Array.map(renderField(form, dispatch)) |> ReasonReact.array;
  //     <Group conf dispatch> children </Group>;
  //   | _ =>
  //     <div>
  //       {{
  //          "Matched a field but not yet implemented: " ++ fieldId;
  //        }
  //        ->ReasonReact.string}
  //     </div>
  //   };
  // | _ => Js.Exn.raiseError("Could not find field " ++ fieldId)
  // };
  <div> "Placeholder for a rendered field: "->ReasonReact.string field.guid->ReasonReact.string </div>;
};

/* Field Constructors */
let newButton = (~config=?, guid) =>
  ok({
    guid,
    fieldType: Button(Button.newButton(guid)),
    common: config |> O.getOr(Common.newCommonConfig(guid) |> getExn),
  });

let newTextInput = (~config=?, guid) =>
  ok({
    guid,
    fieldType: Text(Text.newTextInput(guid)),
    common: config |> O.getOr(Common.newCommonConfig(guid) |> getExn),
  });

let newSelectInput = (~config=?, ~options=?, ~optionValues=?, guid) =>
  (
    switch (options, optionValues) {
    | (None, None) => ok([||])
    | (Some(x), None) => ok(x)
    | (None, Some(x)) => ok(x |> Array.map(y => (Uuid.V1.make(), y)))
    | (Some(_), Some(_)) =>
      Js.Exn.raiseError(
        format("The constructor for Select Box '%s' cannot have both options and optionValues", guid),
      )
    }
  )
  |> kC(opts =>
       ok({
         guid,
         fieldType: Select(Select.newSelectInput(opts, guid) |> Helpers.getExn),
         common: config |> O.getOr(Common.newCommonConfig(guid) |> getExn),
       })
     );
