### git
Rebase current branch on top of master, while preserving commit timestamp:

        git rebase master --committer-date-is-author-date

Rename branch

        git branch -m old_name new_name

Delete branch on github:

        git push -u origin :branch_name

Interactively squash X commits:

        git rebase -i HEAD~X

Squash a ton of stuff:

        git reset --hard HEAD~42
        # HEAD@{1} is where the branch was just before the previous command.
        git merge --squash HEAD@{1}
        git commit

Remove directory:

        git filter-branch --tree-filter 'rm -rf folder-name' --prune-empty -f HEAD 

### hub
Convert issue + branch into pull request:

        hub pull-request -i issue_number -b merge_into -h source_of
