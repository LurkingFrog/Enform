# Initializing the Project

This section is all about how I like to layout a project in "sprint 0" in such a way to ensure at any point,
a new version of the code can be tagged and deployed to production.

This concept is called a [walking skeleton](https://whatis.techtarget.com/definition/walking-skeleton).
Is essence before you write any explicit code in a project, you should have the ability to do a "one-click"
deployment. The reason for this is that most usual reason for missing a project deadline is the integration
of all the pieces in production.

This may seem like a lot of extra work, but in the end it can produce significant savings in both effort and
heartache. Once the skeleton is done, each time you commit a new patch you can be assured that it can all
be deployed if desired.
