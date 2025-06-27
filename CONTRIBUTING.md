# How to contribute

Adding great features and ensuring the widest possible platform compatibility requires community participation.  Anyone wishing to contribute should review these guidelines before submitting pull requests.

## Code of Conduct

Please review and abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Everyone deserves a respectful, adult-oriented environment.

## Celero Core vs Example Experiments

New functionality would reside inside the core project for Celero.  Here it is most important that guidelines are followed to include code formatting, ISO C++ compliance, etc.  The requirements on example experiments are less strict to allow individuals to share their projects the way they wrote them, though adherence to coding standards would be greatly appreciated.

## Getting Started

1. Fork this repo and clone your fork.
2. Ensure you have a GitHub account.
3. Install dependencies and set up your dev environment (see README.md).
4. Search existing [Issues](https://github.com/DigitalInBlue/Celero/issues) or open a new one (use `ISSUE_TEMPLATE.md`):
   * Describe the issue or feature in detail.
   * Include reproduction steps and environment info for bugs.
5. No CLA required—just good faith contributions.

## Making Changes

Change should generally follow the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) branching model.

*   Create a topic branch from where you want to base your work. [Understanding the GitHub Flow](https://guides.github.com/introduction/flow/).
*   All pull requests should be made against the "develop" branch.
*   Make commits of logical units.
*   Check for unnecessary whitespace with `git diff --check` before committing.
*   Make sure your commit messages are in the proper format.
*   Use clang-format to format your files after you make changes.  Use Celero's custom .clang-format file for its configuration.

For details, see ["Writing good commit messages.""](https://github.com/erlang/otp/wiki/Writing-good-commit-messages).

*   Make sure you have added the necessary Google tests for your changes.
*   Run _all_ the tests to assure nothing else was accidentally broken.
*   

### Branching & Workflow

We use two main branches:

* **master**: production-ready code.
* **develop**: integration branch for features and fixes.

**Feature branches** (off `develop`):

```bash
git checkout develop
git pull origin develop
git checkout -b feature/<short-description>
# ...make changes...
git push origin feature/<short-description>
```

**Hotfix branches** (off `master`):

```bash
git checkout master
git pull origin master
git checkout -b hotfix/<short-description>
# ...fix...
git push origin hotfix/<short-description>
```

All PRs (features) target `develop`; hotfixes target `master`.

## Coding Standards & Formatting

* **Language**: ISO-compliant C++.
* **Formatter**: run `clang-format -i` on all `.cpp/.hpp` using Celero’s `.clang-format`.
* **Commit messages**: follow [Conventional Commits](https://www.conventionalcommits.org/).
* **Whitespace**: run `git diff --check` before commit.
* **Docs**: use Doxygen markup for public-facing APIs.

## Writing Tests

* Use [Google Test](https://github.com/google/googletest).
* Cover edge cases; keep tests fast.
* Run all tests:

  ```bash
  cmake --build build && cd build && ctest
  ```

### Documentation

For changes to how a user would run Celero from a command line or how a user might build Celero benchmarks, please provide the appropriate updates to README.md documenting the changes.  In-line code documentaiton following the Doxygen markup language should also be accomplished within the code.

## Submitting Changes

*   Push your changes to a topic branch in your fork of the repository.
*   Submit a pull request to the "develop" branch of the Celero repository.
*   Update your Jira ticket to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge).
*   Include a link to the pull request in the ticket.
*   After feedback has been given we expect responses within two weeks. After two weeks we may close the pull request if it isn't showing any activity.

## Process & Timeline

*   Review goal: within 5 business days.
*   Reply to feedback: within 1 week.
*   Stale PRs: may be closed after 2 weeks of inactivity.   

## Additional Resources

*   ["Writing good commit messages.""](https://github.com/erlang/otp/wiki/Writing-good-commit-messages)
*   [General GitHub documentation](http://help.github.com/)
*   [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/)
*   [GitHub pull request documentation](http://help.github.com/send-pull-requests/)
*   [Understanding the GitHub Flow](https://guides.github.com/introduction/flow/)

