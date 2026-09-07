# Work-package handoff template

Copy this template to `WP-NN-topic.md` before starting an isolated session.

````text
Package:
Base branch and SHA:
Dependencies already merged:
Owned subsystem/files:
Files that must not be changed:
Required behavior:
Acceptance commands:
Final SHA:
Tests and results:
Known gaps:
Next recommended command:
```text

Branches must start from the SHA recorded in the package. If a shared contract
must change, stop the package and submit that change to the integration branch
first; do not independently modify the shared interface in sibling branches.
````
