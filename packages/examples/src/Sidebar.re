/**
 * An example of building a sidebar as an Enform object
 *
 * ## Defaults
 * OnClick should send to the same name
 *
 */
open Enform.Helpers;

//[@@deriving enform_sidebar]
type alertType =
  | ActionOk
  | PageNotFound;

//[@@deriving enform_sidebar]
type sidebar =
  | Alert(alertType)
  | Wizard;

module Sidebar = {
  open Enform.Main;

  let onClick_alert = (id, _evt) => {
    Js.log("Clicked on Alert Button (" ++ id ++ ")");
  };

  let newSidebar = () => {
    Enform.Field.(
      newEnformer()
      ->addFields([|
          newGroup([], "SidebarRootGroup"),
          newButton("Wizard"),
          newButton("ActionOk"),
          newButton("PageNotFound"),
          newGroup([], "Alert"),
        |])
      |> kC(addFieldsToGroup([|"ActionOk", "PageNotFound"|], "Alert"))
    );
  };
};

[@react.component]
let make = () => {
  let sidebar = Sidebar.newSidebar() |> getExn;
  Js.log(sidebar);
  <div className=[%tw "app-sidebar"]> "Placeholder for Examples"->ReasonReact.string </div>;
};
