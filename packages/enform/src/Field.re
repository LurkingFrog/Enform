/** Field managers  */

/** A generic wrapper for the individual fields
 *
 * A group is a subset of fields that work together in common events like validation or visibility.
 *
 * An example is if we want to break a large set of forms into sections. Each section can be validated and
 * saved individually, even though the form as a whole is not valid until complete.
  */
type fieldType =
  | Group(Group.t)
  | Button(Button.t)
  | Select(Select.t)
  | Text(Text.t)
and t = {
  guid: string,
  fieldType,
  common: Common.t,
};

[@react.component]
let make = (~form, ~fieldId, ~reducer) => {
  Js.log("Rendering Enform field ... " ++ fieldId);
  Js.log(form);
  Js.log(reducer);
  // Get the field
  // switch (Form.getField(fieldId, form)) {
  // | Ok(field) =>
  //   Js.log(field);
  //   switch (field.fieldType) {
  //   | Group(conf) =>
  //     let children = conf.fieldGuids |> Array.map(renderField(form, reducer)) |> ReasonReact.array;
  //     <Group conf reducer> children </Group>;
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
  <div> "Placeholder for a rendered field"->ReasonReact.string </div>;
};

/* Field Constructors */

// let newSelectorGroup = (~onChange=Group.Selector.defaultOnChange, ~fieldGuids=[||], selector, guid) => {
//   {
//     guid,
//     fieldType:
//       Group({
//         guid,
//         groupType:
//           Selector({
//             selector: selector.guid,
//             managedFields: fieldGuids,
//             // Standard events
//             onChange,
//           }),
//         memberGuids,
//       }),
//     common: Common.newCommon(guid),
//   };
// };

let newButton = guid => {
  guid,
  fieldType: Button(Button.newButton(guid)),
  common: Common.newCommon(guid),
};

let newTextInput = guid => {
  guid,
  fieldType: Text(Text.newTextInput(guid)),
  common: Common.newCommon(guid),
};

let newSelectInput = (options, guid) => {
  guid,
  fieldType: Select(Select.newSelectInput(options, guid) |> Helpers.getExn),
  common: Common.newCommon(guid),
};
