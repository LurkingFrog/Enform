/** The HTML Select Input
 *
*/

type selectOption = {
  value: string,
  display: option(string),
};

type t = {
  guid: string,
  options: array(selectOption),
};

let newSelectInput = (opts, guid) => {
  guid,
  options: opts |> Array.map((value: string) => {value, display: None}),
};
