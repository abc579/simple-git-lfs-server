# simple-git-lfs-server

This is a very simple and example server that implements part of the Git LFS API.

Since this is a test and I am very unexperienced, it supports the basic things to download and upload lfs objects with SSH authentication.

# Features.

1. Supports SSH authentication.

# Q&A

- Q: Why are they (reference git lfs Go server) using a database?
- A: The database can be helpful in different ways. For now, the most interesting feature that I have seen is the ability to query from a browser the section /mgmt/objects to actually see how many objects the server has. That information is stored in the database. Additionally, they use it to find if the object is stored in the server. This is an ellegant way of checking that a file exists or not. Hadn't it been like that, they would have to check using filepaths.

- Q: How do they handle the {oid} routes?
- A: Doing that manually can be painful; perhaps we should get something already done.

- Q: Should I create an HTTP server from scratch?
- A: That would not be a good idea because that can take too much time.

- Q: What's with the URL's?
- A: The Git LFS client will access the URL: {project}/{repo}/objects/batch. It is in here where we have to parse the JSON; do the correspoding validations, store them in memory and so forth.

- Q: How are we going to name the directories and files?
- A: “content-addressable”  + hash

- Q: What is a tus server?
- A: tus is a new open protocol for resumable uploads built on HTTP.

- Q: Why do we want users?
- A: tbd.

# TODO

- [X] Create HTTP server that supports POST requests. Library.
- [X] Handle HTTP POST request and call a given function, i.e. batch_handler().
- [X] Unpack the contents of the HTTP POST request in batch_handler().
- [X] Define my own JSON class as an interface to the library.
- [X] Create the entity batch_response, this will store the actual response to the API call (/batch/objects).

# Milestones.

- [X] Create an HTTP server and handle the batch API request on /objects/batch. Respond always with a 200 code status but with a JSON saying that the object was not found.
- [X] Decode the request onto a JSON.
- [X] Decode that JSON into an object.
- [X] Process a "download" operation and return always not found for now.
- [X] Process multiple objects in the response.
- [X] Implement download operation.
- [] Impement range http request for download operations.the same thing.
- [] Implement upload operation.
- [] Refactor every part of the code that is using directly features of libraries.
- [] Don't use header files for libraries, compile them into an .so or .a and use its functions.
