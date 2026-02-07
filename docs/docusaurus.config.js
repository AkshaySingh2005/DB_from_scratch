// @ts-check
// `@type` JSDoc annotations allow editor autocompletion and type checking
// (when paired with `@ts-check`).
// There are various equivalent ways to declare your Docusaurus config.
// See: https://docusaurus.io/docs/api/docusaurus-config

import {themes as prismThemes} from 'prism-react-renderer';

// This runs in Node.js - Don't use client-side code here (browser APIs, JSX...)

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'Building Database from scratch in C++',
  tagline: 'A deep dive into database internals with C++',
  url: 'https://AkshaySingh2005.github.io', 
  baseUrl: '/DB_from_scratch/', // Add the leading and trailing slashes here!
  organizationName: 'AkshaySingh2005', 
  projectName: 'DB_from_scratch', 
  deploymentBranch: 'gh-pages',
  trailingSlash: false, // Recommended for GitHub Pages
  // ...
};

export default config;
