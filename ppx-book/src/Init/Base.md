# Base

This contains the primitives and helper functions. Most of the derive functions I've written are recursively
processing the attributes of a record/object. The base contains all of the implementations of primitive data
types that the more complicated records can use.

In addition, separating this from the derive allows users to code their own custom implementations when the
derive doesn't work.
