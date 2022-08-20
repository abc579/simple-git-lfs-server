# simple-git-lfs-server

This is a very naïve implementation of the Git LFS API.

The objective of this project is to learn C++ by doing.

# Very Important (but you can skip this section, hehe)

I am new to C++ and only have experience in database programming (PL
SQL, SQL and so forth), so I apologise if the quality of this code
-especially the directory structure- is terrible-bad.

I am eager to learn, so, if you see something _really_ bad or you -for
some reason- decided to take a look at my code and have any suggestions,
please feel free to contact me.

This is a list of things that I know look _unpleasant_ -to say the
least- but I do not know how to fix them yet. Feel free to skip this
section:

- Makefiles: some days ago I had no idea that this tool existed. I tried
  my best to avoid repeating myself but I think I made quite a mess in
  the process. I know that some folks use tools like cmake and stuff
  like that but I really wanted to learn GNU Make before jumping to
  anything else since it looks very good and has great documentation.

- Dependencies: _yes, I know._ Putting the header and cpp files in the
  *third_party* folder does not seem like a good idea. However, I surfed
  the Internet trying to find a dependency management system that was
  standard and found so many tools: conan.io, vcpkg, hunter... So I
  decided to just leave these files there for now until I know how to
  address this. _Note:_ I was not familiar with dependency management
  systems since in PL SQL everything is embedded in the Oracle Database.

- Tests: _yes, that too._ I have seen some frameworks like GoogleTest;
  looks good. I might add it in the future.

- Project Directory Structure: _I really have no idea_ how to address
  this. I decided to take a look at C++ GitHub repositories but I found
  so many different forms and styles that I ended up very confused.
  
- Decoupling: the ideal thing to do is to avoid having references to
  types that come from libraries. This way, if tomorrow we decide to
  change the JSON or the http server library to some other or even
  change versions, we would have to modify the program in just a few
  places. However, if we change them right now, we would have to modify
  in N places.  I know this is very important but I could not address
  this yet because I need to think about it more -yes, I am not very
  smart.

# Questions

- [X] Investigate how opening a file works and understand it. For instance, why are we using a vector of unsigned chars? What is up with iterators?
- [ ] What happens when we remove objects? Do they have to be deleted from the server too?
- [ ] How do people really manage dependencies in C++? I do have two for now and the way I proceeded does not look promising. I saw some like conan.io, vcpkg, etc.
- [ ] How do people structure source code and header files?  What paradigms should we use here?
- [ ] Why does the API not require auth for the verify callback? Furthermore, the reference Go server crashes if the object does not exist...
- [ ] Regarding authentication, do I only have to check the "Authorization" header? I do not quite understand the ssh part.
- [ ] By implementing the range header, is there a risk of too much I/O operations? Should I implement this?
- [ ] Is there a way to not chase virtual pointers and still keep the same level of flexibility in the code?

# Not Supported

- File locking API.
- Multiple users.
- Transfer adapters that are not "basic".
- Authentication methods that are not "Basic".

# Dependencies

- G++ compiler that supports std=c++14.
- GNU make.
- Git.
- Git LFS client.
- Libraries: libpthread, libssl and libcrypto.
- Openssl cert and private key before running the executable.

# Instructions.

0. Install dependencies:
```
# Debian
sudo apt install libpthread-stubs0-dev libssl-dev git git-lfs g++ make
```
1. Clone this repository somewhere:
```
cd
git clone https://github.com/abc579/simple-git-lfs-server.git
```
2. `cd` to the repository:
```
cd simple-git-lfs-server
```
3. Create your own certificate and key since we are using an https server and we need them:
```
openssl req -x509 -sha256 -nodes -days 2100 -newkey rsa:2048 -keyout mine.key -out mine.crt
```
4. Run `make -j N`. In my PC I use N=15.
5. Modify the file `load_env.sh` according to your needs. It contains
   environment variables that the server will use at runtime.
6. Run `source load_env.sh && ./out/lfs_server`
7. That was the hardest part; now the server will listen for incoming
   http requests using the environment variables provided before.
8. Modify your file `~/.gitconfig` and tell Git LFS to use our own server.
   The property `sslverify` is set to `false` because our cert is a self signed one.
```
  [lfs]
    url = "https://localhost:9999/"

  [http]
    sslverify = false
```
9. Create your own git repository, for example, imagine we had a repository in a directory
   called `test`, which was initialised before and is empty. We can do:
```
cd
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
10. At this point, the Git LFS client will prompt us to type the user
    and password (which we specified before in the `load_env.sh` file).
11. Type your credentials and press return.
12. If everything goes fine, we should see a text pointer with three lines in the _remote repository_. Something like this:
```
version https://git-lfs.github.com/spec/v1
oid sha256:1c2b747b592d66024d63e31178bd8270a95cb5f46db0101bc987d4d0b651b41c
size 24642
```
13. Now, if we check the file directory (which we specified in the `LFS_FILE_DIRECTORY` variable in the `load_env.sh` file)
    we should see that the _actual_ object was stored in it. For example:
```
cd simple-git-lfs-server/files
ls -l
25K Aug 15 03:36 1c2b747b592d66024d63e31178bd8270a95cb5f46db0101bc987d4d0b651b41c
```
14. Finally, we can clone our repository to check that we are getting
    the contents of our files correctly:
```
cd
git clone https://[...] testing_abc
cd testing_abc
```
The Git LFS client will prompt you again to type your credentials; after that, it should download the actual objects. If it does not,
you can always pull that particular object by issuing this command: `git lfs pull --include "testing_stuff.bin"` If everything went
fine, you should see the original file now and not the text pointer.

Note: you can run `make clean` to remove all object files and
executables created. _This will not delete the file directory._

Second note: if you do not plan to use the server anymore, do not forget
to remove the file directory where all files are going to be
stored. Remember, the one we specified in `LFS_FILE_DIRECTORY`.

Third note: if you want to stop the server you can always hit CTRL+C or access the path /stop in your browser.

# Worklog

- [X] Create HTTP server that supports POST requests. Library.
- [X] Handle HTTP POST request and call a given function, i.e. batch_handler().
- [X] Unpack the contents of the HTTP POST request in batch_handler().
- [X] Define my own JSON class as an interface to the library.
- [X] Create the entity batch_response, this will store the actual response to the API call (/batch/objects).
- [X] Create an HTTP server and handle the batch API request on /objects/batch. Respond always with a 200 code status but with a JSON saying that the object was not found.
- [X] Decode the request onto a JSON.
- [X] Decode that JSON into an object.
- [X] Process a "download" operation and return always not found for now.
- [X] Process multiple objects in the response.
- [X] Implement download operation.
- [X] The reason why it might not work is because we uploaded the file with Go.
- [X] Implement upload operation.
- [X] Reduce compilation time. Improve GNU Make file.
- [X] Support verification request.
- [X] Setup authentication.
- [X] Split headers and .cpp files.
- [X] Test for errors.
- [X] Fix upload bug.
- [X] Check if the verify thingy is firing.
- [X] Test with big file sizes.
- [X] Test to kill the process while downloading.
- [X] Convert http to https server.
- [X] Refactor Makefile.
- [X] Clean TODOs and FIXMEs.
- [X] Refactor: orientate stuff towards one paradigm but do not mix them.
- [X] Change b64 algorithm.
- [X] Add pthread to Makefiles.
- [X] Support all error codes?...
- [X] How do we calculate expires_in?
- [X] Fix convoluted JSON construction.
- [X] Run static analyser.
- [X] Encapsulate JSON.
- [X] Reorganise some stuff, create a folder with file utils or something like that.
- [X] Rename certain objects.
- [X] Separate some functions from lfs.h.
- [X] Run CPPcheck and fix results.
- [X] Run valgrind to see if we have any leaks.
- [X] Implement not found pages.
- [X] Check use case of performing an edition on a png file and see how commits are managed.
- [X] Try installation in a docker image and prepare dockerfile.
- [X] Make it more extensible.
- [X] Error handling while creating or opening files.
- [ ] Support Google Cloud Storage:
      https://github.com/googleapis/google-cloud-cpp/blob/main/google/cloud/storage/README.md
      I think it is just matter of changing the part where we are going
      to the file system to save and retrieve files. Now, the idea is to
      make calls to the Google API using JSONs instead. We will also
      need an OAUTH_TOKEN:
      https://cloud.google.com/storage/docs/authentication#apiauth.

# Thanks To

Even though no one is going to see this repository, I would like to thank:

- People behind Git and Git LFS development.
- The authors of the httplib library.
- The authors of the json11 library.
- Rene Nyffenegger for his base64 code snippet.
