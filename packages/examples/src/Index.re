[%bs.raw {|require("./assets/css/tailwind.css")|}];

open Webapi.Dom;

let unwrap = (value: option('a)): 'a => Belt.Option.getExn(value);

let body = document->Document.asHtmlDocument->unwrap->HtmlDocument.body->unwrap;
let head = document->Document.asHtmlDocument->unwrap->HtmlDocument.head;

let makeContainer = text => {
  let container = Document.createElement("div", document);
  container->Element.setClassName("RootContainer");
  container->Element.setId(text);

  let () = container->Element.appendChild(body);
  container;
};

module App = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();

    // switch (session->Session.isLoggedIn) {
    let body =
      switch (url.path->List.nth_opt(0)->Belt.Option.getWithDefault("")->String.lowercase_ascii) {
      | "/"
      | ""
      | "simple" => <SimpleForm />
      | "alert" => <ActionAlert msg="A simple alert type form'" />
      | path => <ActionAlert msg={"There is no page called '" ++ path ++ "'"} />
      };
    <div> <Sidebar /> body </div>;
  };
};

module Root = {
  [@react.component]
  let make = () => {
    <div> <App /> </div>;
  };
};

ReactDOMRe.render(<Root />, makeContainer("root"));
