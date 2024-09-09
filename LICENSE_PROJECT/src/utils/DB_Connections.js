'use strict';

const couchbase = require('couchbase');

class DB_Connections {

    constructor(process_env) {
        this.bucket = null;
        this.process_env = process_env;
    }

    async connect() {

        var process_env = this.process_env;

        const cluster = await couchbase.connect(process_env.DB_CLUSTER_ADDRESS, {
            username: process_env.DB_LICENSE_BUCKET_UNAME,
            password: process_env.DB_LICENSE_BUCKET_PASSWORD
        });

        if (cluster) {

            const cb_bucket = await cluster.bucket(process_env.DB_LICENSE_BUCKET_NAME);
            return cb_bucket;
        }
        else {
            console.log(`Failed to connect to ${process_env.DB_CLUSTER_ADDRESS}`);
        }
    }
}

module.exports = DB_Connections;