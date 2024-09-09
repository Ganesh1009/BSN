<p align="center">
  <a href="https://bionic-h2020.eu/" target="blank"><img src="bionic_logo_header_grad.png" width="300" alt="BIONIC Logo" /></a>
</p>
  
# BIONIC AI Chip Interface

## Description

git: [bionic-ai-chip-interface-repository](https://git.opendfki.de:bionic/h2020/bionic-ai-chip-interface.git) 

Backend application to communicate with the AI chip in [BIONIC](https://bionic-h2020.eu/).

## Installation

```bash
# install node modules
$ npm install
# create ssl certificates and deploy to dist/certs
$ ./create_certificates.sh
```

## Running the app

```bash
# development
$ npm run start

# watch mode
$ npm run start:dev

# production mode
$ npm run start:prod
```

### change PORT

The default port exposed by the application is 3000 it can be overriden by setting the PORT env variable:

```bash
PORT=3030 npm run start:prod
```



## Test

```bash
# unit tests
$ npm run test

# e2e tests
$ npm run test:e2e

# test coverage
$ npm run test:cov
```

## Support

The project uses Nest, it's an MIT-licensed open source project. It can grow thanks to the sponsors and support by the amazing backers. If you'd like to join them, please [read more here](https://docs.nestjs.com/support).

## Stay in touch

- Author - [Mathias Musahl](https://www.mathiasmusahl.de)
- Website - [https://bionic.eu](https://bionic.eu/)
 
## License

  [Licensed under the EUPL-1.2-or-later](LICENSE). 
