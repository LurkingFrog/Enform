# Tests

Testing is how we know the current cycle of writing code is done and ready for user acceptance. My personal
method of Test Driven Development (TDD) is to try and add an integration level test which specifically targets
the current feature I'm writing. It should run quickly and be totally automated to run in the background every
time I save my code.

## Hot Reloading

Integration testing can be challenging when the APIs are being worked on. More time ends up being spent
fixing breaking changes in the tests than thinking about the new code. This is doubly true when working
with a web application.

Because of this, hot reloading is an invaluable tool. Much like only compiling what has changed,
this allows us to change library code and have it updating an open web browser window without changing the
underlying state. Sometimes it really is just faster to manually click a button than it is to run a test.
