/** The HTML Select Input
 *
 * This does a fancy menu instead of a raw <select> tag.
 *
 * TODO: Separate the fancy UI/UX from this code.
*/
open Helpers;

type selectOption = {
  value: string,
  display: string,
};

/** This is an example method for creating a drop-box with finite options
 *
 * TODO: Change this to use a text box as a filter. Code for the filter should be in Filt.re
*/
module Options = {
  type t = {lookup: Belt.HashMap.String.t(selectOption)};

  let addOption = (opts, value, display) => {
    Belt.HashMap.String.set(opts.lookup, value, {value, display});
    ok(opts);
  };

  let newOptions = () => {lookup: Belt.HashMap.String.make(~hintSize=10)};

  /** Get all the options */
  let getOptions = opts => opts.lookup |> Belt.HashMap.String.valuesToArray;
};

type t = {
  // Field Data
  guid: string,
  options: Options.t,
  currentValue: option(string),
  // Display options
  /** The value that should be selected when the field is first loaded */
  defaultValue: option(string),
};

let newSelectInput = (~defaultValue=?, ~_valueFilter=?, opts, guid) => {
  opts
  |> Array.fold_left(
       (acc, (value, display)) => acc |> kC(acc => Options.addOption(acc, value, display)),
       ok(Options.newOptions()),
     )
  |> kC(options => ok({guid, currentValue: None, options, defaultValue}));
};

/** How we display the current value. When we change to filter, this can become a text box */
module Selected = {
  [@react.component]
  let make = (~selected) => {
    <WithProps props={"data-toggle": "dropdown", "aria-expanded": "false", "title": selected.display}>
      <button type_="button" className=[%tw "btn dropdown-toggle btn-default"]>
        <span className=[%tw "filter-option pull-left"]> selected.value->ReasonReact.string </span>
        "&nbsp;"->ReasonReact.string
        <span className=[%tw "caret"] />
      </button>
    </WithProps>;
  };
};

module Menu = {
  [@react.component]
  let make = (~selected, ~options) => {
    let opts =
      options
      |> Array.map(opt => {
           let isSelected = opt.value == selected.value ? [%tw "selected"] : "";
           <li rel="0" className=isSelected>
             <div className="">
               <span className=[%tw "text"] />
               <i className=[%tw "glyphicon check-mark"] />
             </div>
           </li>;
         })
      |> ReasonReact.array;
    <div className=[%tw "dropdown-menu open"]>
      <ul className=[%tw "dropdown-menu inner"] role="menu"> opts </ul>
    </div>;
    // div  style="max-height: 245px; overflow: hidden; min-height: 121px;">
    // ul style="max-height: 243px; overflow-y: auto; min-height: 119px;">
  };
};

[@react.component]
let make = (~field, ~dispatch) => {
  Js.log(dispatch);
  let opts =
    field.options
    |> Options.getOptions
    |> Array.map(opt => <option value={opt.value}> opt.display->ReasonReact.string </option>)
    |> ReasonReact.array;

  let selected =
    (
      switch (field.currentValue, field.defaultValue) {
      | (Some(current), _) => current
      | (_, Some(default)) => default
      | _ =>
        Js.Exn.raiseTypeError("Tried to render a select box without either a current value or a default")
      }
    )
    |> (
      value =>
        HM.find(
          ~notFound=Some(format("Could not find selected value '%s' in possible options", value)),
          field.options.lookup,
          value,
        )
        |> getExn
    );

  <div className=[%tw "col-md-6 col-xs-12"]>
    <select className=[%tw "form-control"] style={ReactDOMRe.Style.make(~display="none", ())}>
      opts
    </select>
    <div className=[%tw "btn-group bootstrap-select form-control"]>
      <Selected selected />
      <Menu selected options={field.options |> Options.getOptions} />
    </div>
    <span className=[%tw "help-block"]> "Select box example"->ReasonReact.string </span>
  </div>;
};
