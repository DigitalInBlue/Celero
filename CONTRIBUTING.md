# How to contribute

Adding great features and ensuring the widest possible platform compatibility requires community participation.  Anyone wishing to contribute should review these guidelines before submitting pull requests.

## Celero Core vs Example Experiments

New functionality would reside inside the core project for Celero.  Here it is most important that guidelines are followed to include code formatting, ISO C++ compliance, etc.  The requirements on example experiments are less strict to allow individuals to share their projects the way they wrote them, though adherence to coding standards would be greatly appreciated.

## Getting Started

* Make sure you have a [GitHub account](https://github.com/signup/free)
* Submit a ticket for your issue, assuming one does not already exist.
  * Clearly describe the issue including steps to reproduce when it is a bug.
  * Make sure you fill in the earliest version that you know has the issue.
* Fork the repository on GitHub

## Making Changes

Change should generally follow the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) branching model.

* Create a topic branch from where you want to base your work. [Understanding the GitHub Flow](https://guides.github.com/introduction/flow/).
* All pull requests should be made against the "develop" branch.
* Make commits of logical units.
* Check for unnecessary whitespace with `git diff --check` before committing.
* Make sure your commit messages are in the proper format.
* Use clang-format to format your files after you make changes.  Use Celero's custom .clang-format file for its configuration.

For details, see ["Writing good commit messages.""](https://github.com/erlang/otp/wiki/Writing-good-commit-messages).

* Make sure you have added the necessary Google tests for your changes.
* Run _all_ the tests to assure nothing else was accidentally broken.

## Making Trivial Changes

### Documentation

For changes to how a user would run Celero from a command line or how a user might build Celero benchmarks, please provide the appropriate updates to README.md documenting the changes.  In-line code documentaiton following the Doxygen markup language should also be accomplished within the code.

## Submitting Changes

* Push your changes to a topic branch in your fork of the repository.
* Submit a pull request to the "develop" branch of the Celero repository.
* Update your Jira ticket to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge).
  * Include a link to the pull request in the ticket.
* After feedback has been given we expect responses within two weeks. After two weeks we may close the pull request if it isn't showing any activity.

# Additional Resources

* ["Writing good commit messages.""](https://github.com/erlang/otp/wiki/Writing-good-commit-messages)
* [General GitHub documentation](http://help.github.com/)
* [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/)
* [GitHub pull request documentation](http://help.github.com/send-pull-requests/)
* [Understanding the GitHub Flow](https://guides.github.com/introduction/flow/)
