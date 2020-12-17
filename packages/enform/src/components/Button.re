/** A simple, clickable button */

/** Catch the standard types of buttons so we can default properly */
type buttonType =
  | /** Clicking this will submit the current group/form */
    Submit
  | /** Clicking this will clear the current group/form */
    Clear
  | /** A cycle of options, each click will go to the next value as the label */
    Toggle
  | /** Run a custom action on click */
    Other;

type t = {
  guid: string,
  buttonType,
};

let newButton = (~buttonType=Submit, guid) => {guid, buttonType};

[@react.component]
let make = (~conf) => {
  switch (conf.buttonType) {
  | _ => <button className=[%tw "btn btn-default"]> "Placeholder for Button"->ReasonReact.string </button>
  };
};
