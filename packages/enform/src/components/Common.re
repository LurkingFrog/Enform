/** This defines the behavior of the field/group/form. If nested, the most specific handler wins */

type fieldState =
  | /** Nothing has been entered into the given field */
    Empty
  | /** The value is unchanged from the default value */
    Default
  | /** The user has altered the value. */
    Changed
  | /** The current value in the field has been validated */
    Validated
  | /** The field can no longer be changed */
    Locked
  | /** This contains a validation error */
    Error(Errors.t);

module EventHandlers = {
  type t = {onChange: string => Belt.Result.t(unit, Errors.t)};

  let default = () => {
    onChange: value => {
      Js.log("Clicked selector with value: " ++ value);
      Eeyo.ok();
    },
  };
};

type t = {
  name: string,
  fieldState,
  eventHandlers: EventHandlers.t,
  // The following are common to all fields/groups
  // Display (className)
  // Event Handling
  // - Bubble?
};

let newCommon = name => {name, fieldState: Empty, eventHandlers: EventHandlers.default()};
