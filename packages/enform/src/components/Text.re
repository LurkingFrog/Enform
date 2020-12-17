/** The various incarnations of a text input */
open Helpers;

type t = {
  guid: string,
  icon: option(string),
};

let newTextInput = (~icon=None, guid): t => {guid, icon};

[@react.component]
let make = (~conf) => {
  Js.log("Rendering Text: " ++ conf.guid);
  let icon =
    switch (conf.icon) {
    | Some(x) => <span className=[%tw "input-group-addon"]> <span className={format("fa %s", x)} /> </span>
    | None => ReasonReact.null
    };
  <div className=[%tw "input-group"] key={conf.guid}>
    icon
    <input type_="text" className=[%tw "form-control"] />
  </div>;
};
