/** A simple, clickable button */

/** Catch the standard types of buttons so we can default properly */
type buttonType =
  | /** Clicking this will submit the current group/form */
    Submit
  | /** Clicking this will clear the current group/form */
    Clear
  | /** Run a custom action on click */
    Other;

type t = {
  guid: string,
  buttonType,
};

let newButton = (~buttonType=Submit, guid) => {guid, buttonType};
