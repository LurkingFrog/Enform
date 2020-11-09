type t = {guid: string};

let newTextInput = (guid): t => {guid: guid};

[@react.component]
let make = () => {
  <div className=[%tw "form-group"]>
    <label className=[%tw "col-md-3 col-xs-12 control-label"]> "Text Field"->ReasonReact.string </label>
    <div className=[%tw "col-md-6 col-xs-12"]>
      <div className=[%tw "input-group"]>
        <span className=[%tw "input-group-addon"]> <span className=[%tw "fa fa-pencil"] /> </span>
        <input type_="text" className=[%tw "form-control"] />
      </div>
      <span className=[%tw "help-block"]> "This is sample of text field"->ReasonReact.string </span>
    </div>
  </div>;
};
