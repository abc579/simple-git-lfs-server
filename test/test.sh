#!/bin/sh

curl -X POST http://localhost:8080/objects/batch -H "Content-Type: application/json" \
  -d '{ "operation": "download",
        "transfers": [ "basic" ],
        "ref": { "name": "refs/header/main" },
        "objects": [
          {
            "oid": "4955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58",
            "size": 1048576
          }
        ],
        "hash_algo": "sha256"
      }'
