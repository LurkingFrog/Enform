/** Import data example
 *
 */
module GoogleSheet = {
  // [@@deriving enform_component]
  type t = {sheetId: string};
};

// [@@deriving enform_component]
type source =
  | GoogleSheets(GoogleSheet.t)
  | Excel
  | CSV;

type target =
  | Postgres;

/** The Import Request Object */
// [@@deriving enform_form ]
type importData = {
  importSource: source, // Attribute? [@default GoogleSheets]
  importTarget: target,
};

/** Derived from importData
 *
 * This is what the derived code should looklike step one of the PPX is making the interface work
 */
// Name is taken from the request Type
module ImportDataForm = {
  open Enform.Main;
  open Enform.Field;

  let newImportDataForm = () => {
    let sheetIdField = newTextInput("sheetId");
    let sourceField = newSelectInput([|"GoogleSheets", "Excel", "CSV"|], "importSource");
    let sourceGroup = newSelectorGroup(sourceField, "sourceGroup");
    newEnformer()->addFields([|sheetIdField, sourceField, sourceGroup|]);
  };
  [@react.component]
  let make = () => {
    let form = newImportDataForm() |> Enform.Helpers.getExn;
    Js.log("Rendering the ImportDataForm:");
    Js.log(form);
    <div className=[%tw ""]> "Placeholder for ImportDataForm"->ReasonReact.string </div>;
  };
};

[@react.component]
let make = () => {
  <div className="SimpleForm"> <ImportDataForm /> </div>;
};
