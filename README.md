Neogit

Neogit is a minimalistic Git client built with C, providing essential version control functionality from the command line.


Features


Initialize a Repository: Easily start a new Git repository for your project.

Add files and directories changes

Commit Changes: Commit your changes with a simple command.

Branching: Create, list, and switch branches to manage your project's history.

Checkout: Switch between branches or specific commit states.

Log: View commit history with flexible filtering options.


Getting Started


Clone the Repository

git clone https://github.com/your-username/neogit_f.git

Build Neogit

Copy code

cd neogit

make

Initialize a Repository

neogit init


Usage


Initialize a Repository

neogit init

Commit Changes

neogit commit -m "Your commit message here"

neogit commit -s shortcut-name

Create a Branch

neogit branch branch-name

Switch Branch

neogit checkout branch-name

View Commit History

neogit log

Switch to a Specific Commit:

neogit checkout commit-hash

Moves the working tree to the state of the specified commit.

Switch to HEAD:

neogit checkout HEAD

Resets the working tree to the latest commit on the current branch.


