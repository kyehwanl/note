
git reset 
=========
특정 파일만 add 취소

다시 얘기하지만 add에 대한 취소는 reset이다. rm 이 아니다. 
rm을 이용하면 git tracking에서 제외하는 엄청난 결과를 초래할 수 있다


<example of --soft, --mixed>
 * git reset --soft     : index에 유지되기 때문에 status상에서 ‘staged’로 됨 → 직전 commit후 이미 파일이 git add 되어 staged된 상태
 * git reset --mixed    : index에서 삭제되었기 때문에 파일들이 unstaged로 됨 → 직전 commit후 소스파일들만 고쳐진 상태
 * git reset --hard     : 수정된 파일까지 원래대로 돌려진 상태로 감 → 직전 commit후 clean상태


< Table. git reset option effects >
Option    HEAD     Index    Working Directory
---------------------------------------------------
 --soft    Yes
 --mixed   Yes      Yes
 --hard    Yes      Yes      Yes



git index rollback
==================

1. Restoration
in case, git reset --hard 에 대해서 다음과 같이 복구할 수 있고, 
$ git reset --hard HEAD@{1}

git branch -D에 대해서 다음과 같이 복구할 수 있다.
$ git branch <new branch> HEAD@{1}
=> $ git branch mybranch HEAD@{1}


* git revert Vs. reset
git revert should be used to undo changes on a public branch, and 
git reset should be reserved for undoing changes on a private branch.


2. Remove from index
(1) remove files from the index or repository that was added already

    git rm --cached file1 file2 dir/file3
    Or, if you have a lot of files:

    git rm --cached `git ls-files -i --exclude-from=.gitignore`
    git ls-files -i --exclude-from=.gitignore | xargs git rm --cached

(2) add files into .gitignore
(3) re-commit


git stash
=========
Stashing takes the dirty state of your working directory – that is, your modified tracked files and staged changes
saves it on a stack of unfinished changes that you can reapply at any time

1. save
  $ git stash

  $ git stash --patch
          Git will not stash everything that is modified but will instead prompt you interactively 
          which of the changes you would like to stash and which you would like to keep in your working directory

2. list
  $ git stash list


3. rollback
  $ git stash apply
          reapply the one you just stashed by using the command shown

  $ git stash pop :  to apply the stash and then immediately drop it from your stack

  $ git stash apply --index 
          :--index option to tell the command to try to reapply the staged changes. 
          If you had run that instead, you’d have gotten back to your original position
 


4. Creating a Branch from a Stash
  $ git stash branch testchanges
          you’ll get a merge conflict and will have to try to resolve it. 
          If you want an easier way to test the stashed changes again, you can run git stash branch, 
          which creates a new branch for you, checks out the commit you were on when you stashed your 
          work, reapplies your work there

5. git drop
  $ git stash list
  stash@{0}: WIP on master: 049d078 added the index file
  stash@{1}: WIP on master: c264051 Revert "added file_size"
  stash@{2}: WIP on master: 21d80a5 added number to log
  $ git stash drop stash@{0}
  Dropped stash@{0} (364e91f3f268f0900bc3ee613f9f733e82aaed43)


6. diff between stash and other index
  (1) diff between stash{0} and it's origianl parent
  $ git stash show -p stash@{0}


  (2) compares the stash against the commit it is based on
  $ git diff stash@{0}^!
        (c.f.) commit^! is a range specifier which means: this commit, but none of its parents
         - From git-scm.com/docs/git-rev-parse, r1^! includes commit r1 but excludes all of its parents


  (3) diff between master(working tree) and stash{0}
  $ git diff [--name-only] stash@{0} master (or HEAD)


  ** FYI **
        (1),(2) have both same result, but (3)'s diff result are different from (1),(2)
        Because the latter compares the CURRENT working tree or other index, NOT the based parent.



git remote
==========

1.  basic usage
  git remote add srx_origin ssh://kyehwanl@ts1.antd.nist.gov/srx
  git push srx_origin master:debug1
       
  ** Using git remote with "ssh tunnel"
    (1) tunnel setting  : ssh -f -N -L12345:node1.bs1.bgp-srx.emulab2.antd.nist.gov:22 ls5.antd.nist.gov
    (2) open connection : ssh -p 12345 localhost
    (3) remote url set  : git remote set-url origin ssh://localhost:12345/users/kyehwanl/<git dir>
    (4) fetch or pull   : git pull origin <branch>
                        : git checkout -b <new br> origin/<origin br>
    

2. Git remote push

  remote로 push를 할때 remote에 있는 대상이 현재 checkout되고 있는 branch에는 push가 되지 않는다
  따라서 다른 branch에 push해야 된다. 

  - 따로 branch를 만들어서 하는 방법
     git push srx_origin master:debug1 --> remote에 debug1이라는 branch를 만들면서 현재 local의 master branch를 push

  - 기존 브랜치에서 다른 branch를 이용

  - one time remotely push
  git pull <URL> <branch>



  git remote fetch (or pull)
  -->pull을 하기 위해서 fetch를 먼저 한다

    git fetch <origin-name> <branch-name>" brings the branch down locally
    git fetch origin refs/heads/*:refs/heads/* # fetch heads

    Using refspecs explicitly:

    $ git fetch origin +pu:pu maint:tmp ('+' force push )

    This updates (or creates, as necessary) branches pu and tmp in the local repository by fetching 
    from the branches (respectively) pu and maint from the remote repository.
    The pu branch will be updated even if it is does not fast-forward, because it is prefixed 
    with a plus sign; tmp will not be.

        

3.  Checking Out Remote Branches

  If your contribution came from a Git user who set up their own repository,
  pushed a number of changes into it, and then sent you the URL to the repository
  and the name of the remote branch the changes are in, you can add them as a
  remote and do merges locally.
  For instance, if Jessica sends you an e-mail saying that she has a great new
  feature in the ruby-client branch of her repository, you can test it by adding
  the remote and checking out that branch locally:

  $ git remote add jessica git://github.com/jessica/myproject.git
  $ git fetch jessica
  $ git checkout -b rubyclient jessica/ruby-client

  --> (1) fetch한후에는  git push, pull 도 쓸수 있음
      (2) rebase/merge 사용  (pull = fetch + rebase/merge)
                git rebase jessica/ruby-client


  (my project case)
  [root@luka bgp-srx]# git remote add quagga_mod ssh://kyehwanl@localhost:12345/users/kyehwanl/programming/proj-bgp-srx/
  [root@luka bgp-srx]# git fetch quagga_mod
  remote: Counting objects: 207, done.
  remote: Compressing objects: 100% (132/132), done.
  remote: Total 135 (delta 103), reused 1 (delta 1)
  Receiving objects: 100% (135/135), 492.59 KiB | 449 KiB/s, done.
  Resolving deltas: 100% (103/103), completed with 67 local objects.
  From ssh://kyehwanl@localhost:12345/users/kyehwanl/programming/proj-bgp-srx
   * [new branch]      fixes      -> quagga_mod/fixes
   * [new branch]      fixes2     -> quagga_mod/fixes2
   * [new branch]      master     -> quagga_mod/master
   * [new branch]      q-test1    -> quagga_mod/q-test1
   * [new branch]      q-test2    -> quagga_mod/q-test2
   * [new branch]      q-test3    -> quagga_mod/q-test3
   * [new branch]      q-test4    -> quagga_mod/q-test4
   * [new branch]      q-test5    -> quagga_mod/q-test5
   * [new branch]      safe       -> quagga_mod/safe
  [root@luka bgp-srx]# git remote -v
  origin  ssh://kyehwanl@localhost:12345/users/kyehwanl/project-iip-bgp/bgp-srx.git
  quagga_mod      ssh://kyehwanl@localhost:12345/users/kyehwanl/programming/proj-bgp-srx/
  [root@luka bgp-srx]# git branch -a
  * master
    q-test2
    origin/q-test1
    origin/q-test2
    quagga_mod/fixes
    quagga_mod/fixes2
    quagga_mod/master
    quagga_mod/q-test1
    quagga_mod/q-test2
    quagga_mod/q-test3
    quagga_mod/q-test4
    quagga_mod/q-test5
    quagga_mod/safe

  [root@luka bgp-srx]# git checkout -b myfixes quagga_mod/fixes
  Branch myfixes set up to track remote branch refs/remotes/quagga_mod/fixes.
  Switched to a new branch 'myfixes'
  [root@luka bgp-srx]# git log
  commit 566fa88fc9b062568ba47ee40d6a1a0a874206e8
  Author: Kyehwan <kyehwanl@nist.gov>
  Date:   Tue Jun 14 08:22:14 2011 -0400

      - bgp command interface added (bgp_vty.c)
      - before sending update, attribute change and revert (applied update and
          withdraw packet




4. (tracked branch를 제외시키려면)

  Simply delete your remote tracking branch:
  git branch -d -r origin/<remote branch name>


  How can I delete 'remotes/public/master' from the branch list?
  git push public   :master


  This would delete the remote branch named master as Kent Fredric has pointed out.
  To list remote-tracking branches:
  git branch -r


  To delete a remote-tracking branch:
  git branch -rd public/master

  <example>
  [kyehwanl@node5 020-03]$ git branch -d -r origin/debug/vm05
  Deleted remote branch origin/debug/vm05 (was f8ccbce).

  [kyehwanl@node5 020-03]$ git remote show origin
  * remote origin
    Fetch URL: ssh://bgp2.antd.nist.gov/projects/srx
    Push  URL: ssh://bgp2.antd.nist.gov/projects/srx
    HEAD branch: multiple_srx_from_dgram_nonblk
    Remote branches:
      comm_string                    tracked
      debug/srx_server_segv          tracked
      debug/vm05                     new (next fetch will store in remotes/origin)
      master                         tracked
      multiple_srx_from_dgram_nonblk tracked
      …
      regiQ_mthread                  tracked
      Local ref configured for 'git push':
      master pushes to master (local out of date)




5. In case of being rejected with "git push"

  (1) git push -f origin <branch> 

  Usually, the command refuses to update a remote ref that is not an ancestor of the local ref used to overwrite it. 
  This flag disables the check. BUT ** This can cause the remote repository to lose commits; use it with care **


  (2) git pull

  Use git pull to pull the latest changes from the remote repository to your local repository. 
  In this case, pulling changes will require a merge because you have made changes to your local repository

  --> with this way, the final git tree becomes merging tree. 


  (3) use 'git stash'

  STEP 1: git stash your local uncommitted changes on that branch.
  STEP 2: git pull origin branch_name -v to pull and merge to locally committed changes on that branch 
        (give this merge some message, and fix conflicts if any.)
  STEP 3: git stash pop the stashed changes (Then you can make commits on popped files if you want or push 
      already committed changes (STEP4) first and make new commit to files later.)
  STEP 4: git push origin branch_name -v the merged changes




6. Other tips

  (1) push all branches into remote repository
        git push --all origin  (c.f.)git pull --all <name>

    
7. rev-list

    git rev-list --count <REMOTE_BRANCH>..<LOCAL_BRANCH> 
        :how many commit the local branch is behind the remote (and ahead vice versa)

    <example>
    $ git ll
    * 7034ec4 (master) modify master branch
    | * db27c8e (HEAD, br1) test commit 3 after dir copy
    | * 683037b 3rd commit for branch br1
    |/
    * 4d2118d 2nd commit test
    * 1e0c0bf first commit

    $ git rev-list br1..master
    7034ec4f51a6f5db336a980b580c558f119f4158

    $ git rev-list master..br1
    db27c8e280d8556033505d6d8130a1057116888a
    683037b9ce08ef7bc1e5af4f2bc603bbe3252ca6

    $ git rev-list master..br1 --count
    2
    $ git rev-list br1..master --count
    1


Github
=======

1. How to remove a commit on Github
  (1) remove the commit on your local repository. using "git rebase -i" 
       git rebase -i HEAD~2 

  (2) delete the second line within the editor window that pops up.

  (3) git push origin +branchName ('+' force push to GitHub)

  Note: please see alternative to git rebase -i in the comments below—
  git reset --soft HEAD^



2. github Automation
   With the external program, expect, (can be downloaded from 'yum download expect'),
   git push & pull operation can be automated like following:

 
    <sync-github.sh>
    #!/bin/sh -x
    echo "Quaggasrx synchronizing with github..."
    cp -rf /users/kyehwanl/Quagga_test/Proces_Performance/QuaggaSRxSuite/qsrx /tmp/
    cd /tmp/qsrx
    git remote add origin https://github.com/kyehwanl/qsrx_priv
    /users/kyehwanl/Shell_Script/input-expect.sh
    rm -rf /tmp/qsrx
    echo "done"


    <input-expect.sh>
    #!/usr/bin/expect -f
    set timeout -1
    spawn git push --all origin 
    expect "Username*"
    send -- "kyehwanl\r"
    expect "Password*"
    send -- "lkhgit1\r"
    send "\r"
    expect eof



Useful Tips
============

1. search from revisions
  (1) Search through previous revisions whose contents contain a given regexp
   <http://travisjeffery.com/b/2012/02/search-a-git-repo-like-a-ninja/>
        git grep <regexp> $(git rev-list --all)



2. Add a date & Time info into a comment

        git commit --amend -m "$(date +"%D %T")"


3. gitignore tip : 

  (1) remove binary files, not source file from the index

        bin_filename
        !bin_filename.[a-z]  --> this might be a source file which has extension of c or go or etc


  (2) negation examples
   A. 
     **/dir/
     !**/dir/filename.ext or "filename.[a-z]"

   B. 
     *.conf
     !**/srx/**/*.conf

    (explanation) "Prepending an exclamation mark to a pattern negates it.  If a file matches a pattern, but 
    also matches a negating pattern defined later in the file, it will not be ignored."


  (3) Debugging .gitignore pattern
   - If you have complicated .gitignore patterns, or patterns spread over multiple .gitignore files, 
     it can be difficult to track down why a particular file is being ignored. You can use the git check-ignore command 
     with the -v (or --verbose) option to determine which pattern is causing a particular file to be ignored:

    $ git check-ignore -v debug.log
    .gitignore:3:*.log debug.log




4. git clone  
    (1) cloning to a specific folder
    git clone <repo> <directory> 

    (2) cloning a specific tag
    git clone --branch <tag> <repo>

    (3) shallow clone (the history of commits specified by the option ,depth)
    git clone -depth=1 <repo>    ( depth=1, most recent commit)
    







