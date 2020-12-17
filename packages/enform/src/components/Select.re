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

type displayMethod =
  | Basic
  | Button
  | TextFilter
  | Custom;

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
  /** Select the way to render the display */
  displayMethod,
  /** Storage of the selected option */
  currentValue: string,
  /** The value this is set to when the form is loaded/cleared */
  initialValue: string,
  /** Guid of the option to treat as null/empty  */
  noneValue: option(string),
  // Display options
  /** The value that should be selected when the field is first loaded */
  defaultValue: option(string),
};

/** Create a new Select Input
 *
 * @param defaultValue The guid of the option to display when one has yet to be selected
 * @param noneValue The guid to be used as an empty selection. If this doesn't exist, it will be added with a
 *        blank display.
 * @param _value_filter This will control which options are visible at any given time. It will be used at times
 *        such as using a text box filter instead of a dropdown.
 */
let newSelectInput = (~defaultValue=?, ~noneValue=?, ~_valueFilter=?, opts, guid) => {
  (
    switch (opts |> Belt.Array.length) {
    | 0 => err(~msg=format("Cannot create Select Input '%s' with no options", guid), Errors.BadValue)
    | _ => ok(opts)
    }
  )
  |> kC(_ =>
       opts
       |> Array.fold_left(
            (acc, (value, display)) => acc |> kC(acc => Options.addOption(acc, value, display)),
            ok(Options.newOptions()),
          )
     )
  |> kC((options: Options.t)
       // Ensure the noneValue gets created if it doesn't already exist
       =>
         switch (noneValue) {
         | None => ok(options)
         | Some(noneGuid) =>
           switch (HM.find(options.lookup, noneGuid)) {
           | Ok(_) => ok(options)
           | Error(_) => Options.addOption(options, noneGuid, "")
           }
         }
       )
  |> kC((options: Options.t) => {
       let initialValue =
         switch (defaultValue, noneValue) {
         | (None, None) =>
           switch (opts->Belt.Array.get(0)) {
           | Some((optId, _)) => ok(optId)
           | None =>
             Js.Exn.raiseError("Impossible Branch - There is always at least one option by this point")
           }
         | (None, Some(value)) => ok(value)
         | (Some(value), _) =>
           HM.find(
             ~notFound=
               Some(
                 format(
                   "The defaultValue '%s' was not found in the options for Select Input '%s'",
                   value,
                   guid,
                 ),
               ),
             options.lookup,
             value,
           )
           |> kC(_ => ok(value))
         };
       initialValue
       |> kC(_ =>
            ok({
              guid,
              displayMethod: Basic,
              options,
              noneValue,
              currentValue: initialValue |> R.unwrap,
              defaultValue,
              initialValue: initialValue |> R.unwrap,
            })
          );
     });
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
           <li rel="0" className=isSelected key={opt.value}>
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
let make = (~conf, ~dispatch) => {
  Js.log("Rendering Select: " ++ conf.guid);
  let _ = dispatch;
  let opts =
    conf.options
    |> Options.getOptions
    |> Array.map(opt =>
         <option value={opt.value} key={opt.value}> opt.display->ReasonReact.string </option>
       )
    |> ReasonReact.array;

  let selected =
    conf.currentValue
    |> HM.find(
         ~notFound=
           Some(format("Could not find selected value '%s' in possible options", conf.currentValue)),
         conf.options.lookup,
       )
    |> getExn;

  switch (conf.displayMethod) {
  | Basic =>
    <div className=[%tw "form-group"] key={conf.guid}>
      <select className=[%tw "form-control"]> opts </select>
    </div>
  | Button =>
    <div className=[%tw "form-group"] key={conf.guid}>
      <select className=[%tw "form-control"] style={ReactDOMRe.Style.make(~display="none", ())}>
        opts
      </select>
      <div className=[%tw "btn-group bootstrap-select form-control"]>
        <Selected selected />
        <Menu selected options={conf.options |> Options.getOptions} />
      </div>
    </div>
  | _ =>
    <div className=[%tw "form-group"] key={conf.guid}>
      <select className=[%tw "form-control"]> opts </select>
    </div>
  };
};
