/** An example of a simple alert dialog */

/** Add back button functionality since it doesn't seem to be in the router
 *
*/

[@react.component]
let make = (~msg=?) => {
  let url = ReasonReactRouter.useUrl();
  let msg =
    msg->Belt.Option.getWithDefault(
      "Path '" ++ List.fold_left((acc, x) => acc ++ "/" ++ x, "", url.path) ++ "'",
    )
    |> ReasonReact.string;
  <div className="row">
    <div className="col-md-12">
      <div className="error-container">
        <div className="error-code"> "404"->ReasonReact.string </div>
        <div className="error-text"> "page not found"->ReasonReact.string </div>
        <div className="error-subtext">
          "Unfortunately we're having trouble loading the page you are looking for. Please wait a moment and try again or use action below."
          ->ReasonReact.string
          <br />
          msg
        </div>
        <div className="error-actions">
          <div className="row">
            <div className="col-md-6">
              <button
                className="btn btn-info btn-block btn-lg"
                onClick={_evt => ReasonReactRouter.push("/dashboard")}>
                "Back to dashboard"->ReasonReact.string
              </button>
            </div>
            <div className="col-md-6">
              <button
                className="btn btn-primary btn-block btn-lg"
                onClick={_evt => Webapi.Dom.(History.(back(history)))}>
                "Previous page"->ReasonReact.string
              </button>
            </div>
          </div>
        </div>
        <div className="error-subtext">
          "Or you can use search to find anything you need."->ReasonReact.string
        </div>
        <div className="row">
          <div className="col-md-12">
            <div className="input-group">
              <input type_="text" placeholder="Search..." className="form-control" />
              <div className="input-group-btn">
                <button className="btn btn-primary"> <span className="fa fa-search" /> </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>;
};
