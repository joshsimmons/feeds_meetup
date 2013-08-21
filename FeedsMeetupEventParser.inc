<?php
// XXX UTC offset?
// XXX uniqueness!
/**
 * @file
 * Feeds parser class for FeedsMeetupEventParser.
 */

/**
 * Class definition for Meetup Event Parser.
 *
 * Parses XML feeds returned from Meetup API event call
 */
class FeedsMeetupEventParser extends FeedsXPathParserXML {
  
  /**
   * Parse the extra mapping sources provided by this parser.
   *
   * @param $fetcher_result FeedsFetcherResult
   * @param $source FeedsSource
   *
   * @see FeedsParser::parse()
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    $meetup_feed = $fetcher_result->getRaw();
    $result = new FeedsParserResult();

    /**
    * @see common_syndication_parser_parse()
    */
    if (!defined('LIBXML_VERSION') || (version_compare(phpversion(), '5.1.0', '<'))) {
      @$sxml = simplexml_load_string($meetup_feed, NULL);
    }
    else {
      @$sxml = simplexml_load_string($meetup_feed, NULL, LIBXML_NOERROR | LIBXML_NOWARNING | LIBXML_NOCDATA);
    }

    // Got a malformed XML.
    if ($sxml === FALSE || is_null($sxml)) {
      throw new Exception(t('FeedsMeetupEventParser: Malformed XML source.'));
    }

    // Run parsing
    $result = $this->parseMeetup($sxml, $source, $fetcher_result);

    return $result;
  }

  /**
   * Add the extra mapping sources provided by this parser.
   */
  public function getMappingSources() {
    return parent::getMappingSources() + array(
      'guid' => array(
        'name' => t('Event ID (GUID)'),
      ),
      'title' => array(
        'name' => t('Event title'),
        'description' => t('Event title.'),
      ),
      'event_id' => array(
        'name' => t('Event ID'),
        'description' => t('Meetup event unique ID.'),
      ),
      'event_timestamp' => array(
        'name' => t('Event time (Timestamp)'),
      ),
      'event_datetime' => array(
        'name' => t('Event time (Datetime)'),
      ),
      'utc_offset' => array(
        'name' => t('UTC offset'),
        'description' => t('Offset of timestamps from UTC.'),
      ),
      'updated_timestamp' => array(
        'name' => t('Updated on (Timestamp)'),
      ),
      'updated_datetime' => array(
        'name' => t('Updated on (Datetime)'),
      ),
      'published_timestamp' => array(
        'name' => t('Published on (Timestamp)'),
      ),
      'published_datetime' => array(
        'name' => t('Published on (Datetime)'),
      ),
      'description' => array(
        'name' => t('Description'),
      ),
      'event_url' => array(
        'name' => t('Event URL'),
        'description' => t('The URL of the event.'),
      ),
      'rsvp_yes' => array(
        'name' => t('"Yes" RSVP count'),
      ),
      'rsvp_maybe' => array(
        'name' => t('"Maybe" RSVP count'),
      ),
      'waitlist' => array(
        'name' => t('Waitlist count'),
      ),
      'headcount' => array(
        'name' => t('Headcount'),
      ),
      'group_name' => array(
        'name' => t('Group name'),
        'description' => t('The name of the hosting Meetup group.'),
      ),
      'group_url' => array(
        'name' => t('Group URL'),
        'description' => t('The URL of the hosting Meetup group.'),
      ),
      'group_id' => array(
        'name' => t('Group ID'),
        'description' => t('The ID of the hosting Meetup group.'),
      ),
      'group_pronoun' => array(
        'name' => t('Group pronoun'),
        'description' => t('The pronoun the hosting Meetup group uses to describe its members.'),
      ),
      'venue_name' => array(
        'name' => t('Venue name'),
      ),
      'venue_id' => array(
        'name' => t('Venue ID'),
      ),
      'address_1' => array(
        'name' => t('Address line 1'),
      ),
      'address_2' => array(
        'name' => t('Address line 2'),
      ),
      'city' => array(
        'name' => t('City'),
      ),
      'zip' => array(
        'name' => t('Zip'),
      ),
      'state' => array(
        'name' => t('State'),
      ),
      'country' => array(
        'name' => t('Country'),
      ),
      'lat' => array(
        'name' => t('Latitude'),
      ),
      'lon' => array(
        'name' => t('Longitude'),
      ),
    );
  }

  /**
   * Parse XML feed
   *
   * @param SimpleXMLElement $sxml
   * @param FeedsFetcherResult $fetcher_result
   * @param FeedsSource $source
   */
  private function parseMeetup(SimpleXMLElement $sxml, FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    // XML was parsed successfully, so we can begin to process items
    $result = new FeedsParserResult();
    $fetcher_result->title = (string) $sxml->head->title;
    $fetcher_result->description = (string) $sxml->head->description;
    $fetcher_result->link = (string) $sxml->head->url;
    $feed_title = (string) $sxml->head->title;

    // Iterate over entries in feed
    foreach ($sxml->xpath('//item') as $entry) {
      if(isset($entry->venue)) {
        $venue_name = (isset($entry->venue->name)) ? (string) $entry->venue->name : '';
        $venue_id = (isset($entry->venue->id)) ? (string) $entry->venue->id : '';
        $address_1 = (isset($entry->venue->address_1)) ? (string) $entry->venue->address_1 : '';
        $address_2 = (isset($entry->venue->address_2)) ? (string) $entry->venue->address_2 : '';
        $city = (isset($entry->venue->city)) ? (string) $entry->venue->city : '';
        $zip = (isset($entry->venue->zip)) ? (string) $entry->venue->zip : '';
        $state = (isset($entry->venue->state)) ? (string) $entry->venue->state : '';
        $country = (isset($entry->venue->country)) ? (string) $entry->venue->country : '';
        $lat = (isset($entry->venue->lat)) ? (string) $entry->venue->lat : '';
        $lon = (isset($entry->venue->lon)) ? (string) $entry->venue->lon : '';
      } else {
        $venue_name = '';
        $venue_id = '';
        $address_1 = '';
        $address_2 = '';
        $city = '';
        $zip = '';
        $state = '';
        $country = '';
        $lat = '';
        $lon = '';
      }

      $item = array(
        'guid' => (string) $entry->id,
        'title' => (string) $entry->name,
        'event_id' => (string) $entry->id,
        'event_url' => (string) $entry->event_url,
        'description' => (string) $entry->description,
        'headcount' => (string) $entry->headcount,
        'waitlist' => (string) $entry->waitlist_count,
        'rsvp_yes' => (string) $entry->yes_rsvp_count,
        'rsvp_maybe' => (string) $entry->maybe_rsvp_count,
        'venue_name' => (string) $venue_name,
        'venue_id' => (string) $venue_id,
        'address_1' => (string) $address_1,
        'address_2' => (string) $address_2,
        'city' => (string) $city,
        'zip' => (string) $zip,
        'state' => (string) $state,
        'country' => (string) $country,
        'lat' => (string) $lat,
        'lon' => (string) $lon,
        'group_id' => (string) $entry->group->id,
        'group_name' => (string) $entry->group->name,
        'group_pronoun' => (string) $entry->group->who,
        'group_url' => 'http://www.meetup.com/' . $entry->group->urlname,
        'utc_offset' => (string) $this->msToSeconds($entry->utc_offset),
        'event_datetime' => date('Y-m-d H:i:s', strtotime($this->msToSeconds($entry->time))),
        'event_timestamp' => strtotime($this->msToSeconds($entry->time)),
        'updated_datetime' => date('Y-m-d H:i:s', strtotime($this->msToSeconds($entry->updated))),
        'updated_timestamp' => strtotime($this->msToSeconds($entry->updated)),
        'published_datetime' => date('Y-m-d H:i:s', strtotime($this->msToSeconds($entry->created))),
        'published_timestamp' => strtotime($this->msToSeconds($entry->created)),
      );

      // Populate the FeedsFetcherResult object with the parsed results.
      $result->items[] = $item;
    }
    return $result;
  }

  /**
   *  Adjust millisecond Unix timestamp to second.
   *
   *  @param $seconds
   *    The number of seconds to display.
   */
  public function msToSeconds($milliseconds) {
    return $milliseconds / 1000;
  }

  /**
   *  Adjust timestamp to UTC. 
   * XXX do I need to detect default timezone?
   * XXX depends on how field is stored ... we'll see
   *
   *  @param $seconds
   *    The number of seconds to display.
   */
  public function offsetTime($time, $offset) {
    
    return $time;
  }
}
