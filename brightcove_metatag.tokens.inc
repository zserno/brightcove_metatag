<?php

/**
 * @file
 * Token integration for the Brightcove Video Connect module.
 */

/**
 * Implements hook_token_info().
 */
function brightcove_metatag_token_info() {
  $type = array(
    'name' => t('Brightcove'),
    'description' => t('Tokens related to the default Brightcove video field (field_bc).'),
  );

  // Core tokens for Brightcove.
  $bc = array();
  $bc['video_id'] = array(
    'name' => t("Brightcove video ID"),
    'description' => t("The unique Brightcove Video ID."),
  );
  $bc['name'] = array(
    'name' => t("Name"),
    'description' => t("The title of the Brightcove video."),
  );
  $bc['description'] = array(
    'name' => t("Description"),
    'description' => t("The short description of the Brightcove video."),
  );
  $bc['tags'] = array(
    'name' => t("Tags"),
    'description' => t("A comma-separated list of tags of the Brightcove video."),
  );
  $bc['thumbnail'] = array(
    'name' => t("Thumbnail"),
    'description' => t("URI of the thumbnail image of the Brightcove video."),
  );

  return array(
    'types' => array('brightcove' => $type),
    'tokens' => array('brightcove' => $bc),
  );
}

/**
 * Implements hook_tokens().
 */
function brightcove_metatag_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'brightcove' && !empty($data['node'])) {
    $node = $data['node'];

    // Fetch first item of field_bc.
    $field_items = field_get_items('node', $node, 'field_bc');
    $field = reset($field_items);

    // Load the video object.
    $bcid = $field['bcid'];
    $video_id = $field['brightcove_id'];

    $client = entity_load_single('brightcove_client', $bcid);
    $video = brightcove_load_video($video_id, $client);

    foreach ($tokens as $name => $original) {
      switch ($name) {
      case 'video_id':
        $replacements[$original] = $video->getID();
        break;

      case 'name':
        $replacements[$original] = $sanitize ? check_plain($video->getName()) : $video->getName();
        break;

      case 'description':
        $replacements[$original] = $sanitize ? check_plain($video->getDescription()) : $video->getDescription();
        break;

      case 'tags':
        $tags = implode(', ', $video->getTags());
        $replacements[$original] = $sanitize ? check_plain($tags) : $tags;
        break;

      case 'thumbnail':
        $images = $video->getImages();
        $replacements[$original] = $images['thumbnail']->getSrc();
        break;
      }
    }
  }

  return $replacements;
}
