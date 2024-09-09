# the first image use full node image as the builder because it has everything
FROM node:12 as builder

WORKDIR /build

COPY ["package.json", "package-lock.json*", "npm-shrinkwrap.json*", "./"]

RUN npm ci

COPY . .

## compile typescript
RUN npm run build

## remove packages of devDependencies
RUN npm prune --production

FROM node:12-alpine
ENV NODE_ENV production
WORKDIR /app

## Copy the necessary files form builder
COPY --from=builder "/build/dist/" "./dist/"
COPY --from=builder "/build/node_modules/" "./node_modules/"
COPY --from=builder "/build/package.json" "./package.json"

EXPOSE 3000
CMD ["npm", "run", "start:prod"]
