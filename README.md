# brightcove_metatag
Provides basic token integration for the [Brigthcove Video Connect](https://www.drupal.org/project/brightcove) Drupal module as a patch.

## Usage
1. Apply this patch to the [Brigthcove Drupal module](https://www.drupal.org/project/brightcove):
  * `patch -p1 < brightcove-token-1.patch`
1. Clear Drupal caches
1. Visit /admin/help/token to see all the available tokens of your Drupal site
1. Under Nodes, you'll see the new Brightcove tokens under each brightcove-type field. For example if your field is field_bc then you'll have the following tokens available on a node page:
  * [node:field_bc:video_id]
  * [node:field_bc:name]
  * [node:field_bc:thumbnail]
  * [node:field_bc:tags]
  * [node:field_bc:video_id]

## Example use cases

### 1. Provide a Brightcove image thumbnail for the Open Graph protocol (e.g. Facebook, Pintereset, LinkedIn)
1. Install Metatag and Metatag: OpenGraph modules
1. Visit /admin/config/search/metatags and add default meta tags for your node type if you haven't already
1. Edit that node type's meta tag settings, for example Node: Article
1. Under the Open Graph fieldset add the thumbnail token of your brightcove field to the Image field, e.g. [node:field_bc:thumbnail]
1. Now all your nodes of this type should print the og:image meta tag in the page's `<head>` section
