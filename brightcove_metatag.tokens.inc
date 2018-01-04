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
    'description' => t('Tokens related to Brightcove video fields.'),
    'needs-data' => 'node',
  );

  // Core tokens for Brightcove.
  $brightcove_video['video_id'] = array(
    'name' => t("Brightcove video ID"),
    'description' => t("The unique Brightcove Video ID."),
  );
  $brightcove_video['name'] = array(
    'name' => t("Name"),
    'description' => t("The title of the Brightcove video."),
  );
  $brightcove_video['description'] = array(
    'name' => t("Description"),
    'description' => t("The short description of the Brightcove video."),
  );
  $brightcove_video['tags'] = array(
    'name' => t("Tags"),
    'description' => t("A comma-separated list of tags of the Brightcove video."),
  );
  $brightcove_video['thumbnail'] = array(
    'name' => t("Thumbnail"),
    'description' => t("URI of the thumbnail image of the Brightcove video."),
  );

  return array(
    'types' => array('brightcove' => $type),
    'tokens' => array('brightcove' => $brightcove_video),
  );
}

/**
 * Implements hook_token_info_alter().
 */
function brightcove_metatag_token_info_alter(&$info) {
  $fields = _token_field_info();
  $field_info = field_info_fields();

  // Make `brightcove_field` type fields chained to the brightcove token type.
  foreach ($fields as $field_name => $field) {
    // Look for `brightcove_field` field types.
    if ($field_info[$field_name]['type'] == 'brightcove_field') {
      foreach (array_keys($field['bundles']) as $token_type) {
        $info['tokens'][$token_type][$field_name] = array(
          'name' => $field['label'],
          'description' => $field['description'],
          'type' => 'brightcove',
        );
      }
    }
  }
}

/**
 * Implements hook_tokens().
 */
function brightcove_metatag_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $sanitize = !empty($options['sanitize']);
  $replacements = array();

  $fields = _token_field_info();
  $field_info = field_info_fields();

  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      // Split token up to get node tokens.
      $parts = explode(':', $name, 2);

      if (count($parts) != 2) {
        continue;
      }

      $field_name = $parts[0];
      $name = $parts[1];

      // Check for brightcove field tokens.
      if (!isset($field_info[$field_name]) || $field_info[$field_name]['type'] != 'brightcove_field') {
        continue;
      }

      // Check if this brightcove field is not empty.
      $field_items = field_get_items('node', $node, $field_name);
      if ($field_items === FALSE) {
        continue;
      }

      // Fetch first item of the brightcove field.
      $field = reset($field_items);

      // Load the video object.
      $bcid = $field['bcid'];
      $video_id = $field['brightcove_id'];

      $client = entity_load_single('brightcove_client', $bcid);
      $video = brightcove_load_video($video_id, $client);

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
