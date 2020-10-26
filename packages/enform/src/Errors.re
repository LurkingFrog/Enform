/** Internal Errors
 *
 * This pattern is used so we can consistently handle all errors using the Result (Either) Monad
 */;

/** An enumeration of all the possible system errors */
type t =
  | /** A catchall for errors that aren't explicitly listed */
    General
  | /** Tried to update a second value with the same unique identifier */
    DuplicateKey
  | /** This operation cannot run on the given enumerated type */
    InvalidEnum
  | /** The format entered cannot be parsed to the expected value */
    InvalidFormat
  | /** When an item isn't located in a group */
    NotFound
  | /** Index isn't in the range of the given list/array */
    OutOfRange;
