/** This defines the behavior of the field/group/form. If nested, the most specific handler wins */

type state =
  | /** The item is empty, but the user has not had an opportunity to modify it */
    New
  | /** The item has explicitly been set to an empty value  */
    Empty
  | /** The value is unchanged from the default value */
    Default
  | /** The user has altered the value. */
    Changed
  | /** The current value in the field(s) has been validated */
    Validated
  | /** The item and children can not be be changed */
    Locked
  | /** This contains a validation error */
    Error(Errors.t);

module EventHandlers = {
  type t = {onChange: string => Belt.Result.t(unit, Errors.t)};

  let default = () => {
    onChange: value => {
      Js.log("Clicked selector with value: " ++ value);
      Eeyore.Eeyo.ok();
    },
  };
};

type t = {
  name: string,
  state,
  /** A guid list of the groups that this item belongs to. */
  memberOf: array(string),
  eventHandlers: EventHandlers.t,
  // The following are common to all fields/groups
  // validators: Belt.HashMap.String.t(string => Belt.Result.t((), Eeyo.t)
  // Display (className)
  // Event Handling
  // - Bubble?
};

/** Create a default component configuration */
let newCommon = name => {name, state: New, memberOf: [|"__root"|], eventHandlers: EventHandlers.default()};
