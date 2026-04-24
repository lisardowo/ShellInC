# Welcome to DVshell contributing guide

Thank you for contributing to my project! 

In this guide I will stablish the contribution workflow.

## New contributor?

See the [README](README.md) to get an overview of the project. Here are some helpful resources to get you comfortable with open source contribution:

- [Finding ways to contribute to open source on GitHub](https://docs.github.com/en/get-started/exploring-projects-on-github/finding-ways-to-contribute-to-open-source-on-github)
- [Set up Git](https://docs.github.com/en/get-started/quickstart/set-up-git)
- [GitHub flow](https://docs.github.com/en/get-started/quickstart/github-flow)
- [Collaborating with pull requests](https://docs.github.com/en/github/collaborating-with-pull-requests)

## IMPORTANT

I do not accept PR of corrected code. I mean, lets say you found a flag and patch it Im really happy you did that but unfortunately if you want to PR that patch I wont accept it, Is not cause I don't appreciate your efforts, is cause the purpose of this shell is to keep it as vulnerable as possible so new people can try this and learn.However you can always fill your [win report](templates/eportWin_template) to help other programmers to achieve that victory.

## Getting started

Before writing code and creating PR make sure that it aligns with this guidelines:

- This project is for research and education.
- Your PR must comply with our [Coding Style](STYLE.md)
- Your PR must contain code compatible with project's [LICENSE](LICENSE).


## AI usage

- Usage of any LLM is allowed as a roadmap, for explanation as code reviewer or to generate the diff patch(recommended) BUT NO AI GENERATED CODE WILL BE ACCEPTED 

## So you found another vulnerability to register/implement

### Implement

If you thought and created a whole new vulnerability read here:

1. Fork the repository.
- Using GitHub Desktop:
  - [Getting started with GitHub Desktop](https://docs.github.com/en/desktop/installing-and-configuring-github-desktop/getting-started-with-github-desktop) will guide you through setting up Desktop.
  - Once Desktop is set up, you can use it to [fork the repo](https://docs.github.com/en/desktop/contributing-and-collaborating-using-github-desktop/cloning-and-forking-repositories-from-github-desktop)!

- Using the command line:
  - [Fork the repo](https://docs.github.com/en/github/getting-started-with-github/fork-a-repo#fork-an-example-repository) so that you can make your changes without affecting the original project until you're ready to merge them.

2. Install build requirements

3. Create a working branch and start with your changes!

#### Commit your update

Commit the changes once you are happy with them. Make sure that code compilation is not broken. Check syntax and formatting.

#### Pull Request

- When you're done making the changes, open a pull request, often referred to as a PR. 

- Fill the [pull request Template](templates/pullRequest_Template.md) and upload it with your PR

- Once you submit your PR, I will review your proposal. I may ask questions or request for additional information.

- I may ask for changes to be made before a PR can be merged, either using [suggested changes](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/incorporating-feedback-in-your-pull-request) or pull request comments. You can apply suggested changes directly through the UI. You can make any other changes in your fork, then commit them to your branch.

- As you update your PR and apply changes, mark each conversation as [resolved](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/commenting-on-a-pull-request#resolving-conversations).
- If you run into any merge issues, checkout this [git tutorial](https://lab.github.com/githubtraining/managing-merge-conflicts) to help you resolve merge conflicts and other issues.

#### Your PR is merged!

### Register

If you happened to found a vulnerability undocumented just fill out the [pull request Template](templates/pullRequest_Template.md) and upload it in a new issue with the tag of ~register~


**Im really grateful for your help!**