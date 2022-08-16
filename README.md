# simple-git-lfs-server

This is a very naïve implementation of part of the Git LFS API.
 
# Questions.

- [] Investigate how opening a file works and understand it. For
  instance, why are we using a vector of unsigned chars? What is up with
  iterators?
- [] What happens when we remove objects? Do they have to be deleted
  from the server too?
- [] If we upload objects that already exists, we do not have to do
  anything? Not even replacing them?... Weird.
- [] How do people manage dependencies in C++? I do have two for now and
  the way I proceeded does not look promising. I saw some like conan.io,
  vcpkg, etc.
- [] How do people structure source code and header files? Project
  scaffolding.
- [] How can we speed compilation times?
- [] Why does the API not require auth for the verify callback?
  Furthermore, the reference Go server crashes if the object does not
  exist... In our case, the auth is necessary for verify too.
- [] When I am downloading a repository with very large files, if I kill the
  process in between, the server seems to keep using a lot of CPU, but I
  am kinda lost on how to avoid that.
- [] How come I do not have to do anything for authentication? (besides
  checking "Authorization" header)
- [] Should we implement the http range request? If so, how? If we do not
  cache the files I think there will be a problem with I/O operations.

# Not supported.

- File locking API.
- Multiple users.
- Transfer adapters that are not "basic".
- Authentication methods that are not "Basic".

# Dependencies.

- G++ compiler that supports C++14.
- GNU make.
- Libraries: libpthread, libssl and libcrypto.
- Openssl cert and private key before running the executable.
- Git LFS client.

# Instructions.

1. Install libpthread, libssl, libcrypto and git lfs in your system.
```
# Debian
sudo apt install libpthread-stubs0-dev libssl-dev 
```
2. `cd` into the main directory of the project.
3. Create your own certificate and key since we are using an https server and we need those:
   `openssl req -x509 -sha256 -nodes -days 2100 -newkey rsa:2048 -keyout mine.key -out mine.crt`.
4. Run `make -j N`.
5. Modify the file `load_env.sh` according to your needs.
6. Run `source load_env.sh && ./out/lfs_server`
7. That was the hardest part; now the server will listen for incoming https calls.
8. Modify your file `~/.gitconfig` and tell Git LFS to use our own server:
```
  [lfs]
    url = "https://localhost:8080/"

  [http]
    sslverify = false
```
9. Create your own git repository, for example, imagine we had a repository in a directory called `test`, which was initialised before and it was empty. We can do:
```
cd test
git lfs install
git lfs track "*.bin"
git add .gitattributes
git commit -m 'Tracking bin files'
fallocate -l 2MiB testing_stuff.bin
git add testing_stuff.bin
git commit -m 'New binary file'
git push
```
10. At this point, the Git LFS client will prompt us to type the user and password (which we specified before in the `load_env.sh` file).
11. Type your credentials and hit return.
12. If everything goes fine, we should see a text pointer with three lines in the remote repository, for example, GitHub. Something like this:
```
version https://git-lfs.github.com/spec/v1
oid sha256:1c2b747b592d66024d63e31178bd8270a95cb5f46db0101bc987d4d0b651b41c
size 24642
```
13. Now, if we check the file directory (which we specified in the variable `LFS_FILE_DIRECTORY` in the file `load_env.sh`) we should see the actual object was stored inside it. For example:
```
cd simple-git-lfs-server/files/1c/2b
ls -l
25K Aug 15 03:36 747b592d66024d63e31178bd8270a95cb5f46db0101bc987d4d0b651b41c
```
14. Finally, we can clone our repository to check that we are getting the contents of our files correctly:
```
cd ~
git clone https://[...] testing_abc
cd testing_abc
```
The Git LFS client will prompt you again to type your credentials; after that, it should download the actual objects. If it does not, you can always pull that particular object by issuing this command: `git lfs pull --include "testing_stuff.bin"` If everything went fine, you should see the original file now and not the text pointer.

Note: run `make clean` to remove all object files and executables created.

# Worklog

+ [X] Create HTTP server that supports POST requests. Library.
+ [X] Handle HTTP POST request and call a given function, i.e. batch_handler().
+ [X] Unpack the contents of the HTTP POST request in batch_handler().
+ [X] Define my own JSON class as an interface to the library.
+ [X] Create the entity batch_response, this will store the actual response to the API call (/batch/objects).
+ [X] Create an HTTP server and handle the batch API request on /objects/batch. Respond always with a 200 code status but with a JSON saying that the object was not found.
+ [X] Decode the request onto a JSON.
+ [X] Decode that JSON into an object.
+ [X] Process a "download" operation and return always not found for now.
+ [X] Process multiple objects in the response.
+ [X] Implement download operation.
+ [X] The reason why it might not work is because we uploaded the file with Go.
+ [X] Implement upload operation.
+ [X] Reduce compilation time. Improve GNUMake file.
+ [X] Support verification request.
+ [X] Setup authentication.
+ [X] Split headers and .cpp files.
+ [X] Test for errors.
+ [X] Fix upload bug.
+ [X] Check if the verify thingy is firing.
+ [X] Test with big file sizes.
+ [X] Test to kill the process while downloading.
+ [X] Convert http to https server.
+ [X] Refactor Makefile.
+ [X] Clean TODOs and FIXMEs.
+ [X] Refactor: orientate stuff towards one paradigm but do not mix them.
+ [X] Change b64 algorithm.
+ [X] Add pthread to Makefiles.
+ [X] Support all error codes?...
+ [X] How do we calculate expires_in?
+ [X] Fix convoluted JSON construction.
+ [X] Run static analyser.
+ [X] Encapsulate JSON.
+ [X] Reorganise some stuff, create a folder with file utils or something like that.
+ [X] Rename certain objects.
+ [X] Separate some functions from lfs.h.
+ [X] Run CPPcheck and fix results.
+ [X] Run valgrind to see if we have any leaks.
+ [] Implement Range header.
+ [] Check use case of performing an edition on a png file and see how commits are managed.
