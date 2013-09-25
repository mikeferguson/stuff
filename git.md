### git
Rename branch

        git branch -m old_name new_name

Delete branch on github:

        git push -u origin :branch_name

Interactively squash X commits:

        git rebase -i HEAD~X

### hub
Convert issue + branch into pull request:

        hub pull-request -i issue_number -b merge_into -h source_of
