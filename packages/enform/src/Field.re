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
  // Js.log(field);
  let _ = dispatch;
  // Search: class="([-\w\s]+)"
  // Replace: className=[%tw "$1"]

  let label = field.common.label |> O.getOr("") |> ReasonReact.string;
  let help =
    switch (field.common.help) {
    | Some(txt) => <span className=[%tw "help-block"]> txt->ReasonReact.string </span>
    | None => ReasonReact.null
    };
  let rendered =
    switch (field.fieldType) {
    | Button(conf) => <Button conf />
    | Text(conf) => <Text conf />
    | Select(conf) => <Select conf dispatch />
    };
  <div className=[%tw "form-group"]>
    <label className=[%tw "col-md-3 col-xs-12 control-label"]> label </label>
    <div className=[%tw "col-md-6 col-xs-12"]> rendered help </div>
  </div>;
};

/* Field Constructors */
let newButton = (~config=?, guid) =>
  ok({
    guid,
    fieldType: Button(Button.newButton(guid)),
    common: config |> O.getOr(Common.newCommonConfig(guid) |> getExn),
  });

let newTextInput = (~config=?, ~icon=?, guid) =>
  ok({
    guid,
    fieldType: Text(Text.newTextInput(~icon, guid)),
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
