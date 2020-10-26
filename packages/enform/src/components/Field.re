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

/* Field Constructors */
let newGroup = (fields, guid) => {
  // Check if the fields exists
  let fieldGuids = fields |> List.map(field => field.guid);
  {guid, fieldType: Group(Group.newGroup(~fieldGuids, guid)), common: Common.newCommon(guid)};
};

let newSelectorGroup = (~onChange=Group.Selector.defaultOnChange, ~fieldGuids=[], selector, guid) => {
  {
    guid,
    fieldType:
      Group({
        guid,
        groupType:
          Selector({
            selector: selector.guid,
            managedFields: fieldGuids,
            // Standard events
            onChange,
          }),
        fieldGuids,
      }),
    common: Common.newCommon(guid),
  };
};

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
  fieldType: Select(Select.newSelectInput(options, guid)),
  common: Common.newCommon(guid),
};
