// src/app/api/mongo/route.js
import { connectToMongo } from '../../../lib/mongoConnect';
import mongoose from 'mongoose';
import { ObjectId } from 'mongodb';

export async function POST(request) {
  try {
    const { searchParams } = new URL(request.url);
    const collection = searchParams.get('collection');
    if (!collection) {
      return Response.json({ error: 'Missing `collection` in query params.' }, { status: 400 });
    }

    const { data } = await request.json();
    if (!data || typeof data !== 'object') {
      return Response.json({ error: 'Invalid or missing `data` in body.' }, { status: 400 });
    }

    await connectToMongo();
    const db = mongoose.connection.db;
    const col = db.collection(collection);
    const result = await col.insertOne(data);

    return Response.json({ success: true, insertedId: result.insertedId }, { status: 201 });
  } catch (err) {
    console.error('Mongo POST error:', err);
    return Response.json({ error: 'Database operation failed', detail: err.message }, { status: 500 });
  }
}

export async function GET(request) {
  try {
    const { searchParams } = new URL(request.url);
    const collection = searchParams.get('collection');
    if (!collection) {
      return Response.json({ error: 'Missing `collection` in query params.' }, { status: 400 });
    }

    let query = {};
    const rawQuery = searchParams.get('q');
    if (rawQuery) {
      try {
        const parsed = JSON.parse(rawQuery);

        // Convert _id if present and in $oid format
        if (parsed._id && typeof parsed._id === 'object' && '$oid' in parsed._id) {
          parsed._id = new ObjectId(parsed._id.$oid);
        } else if (parsed._id && typeof parsed._id === 'string') {
          parsed._id = new ObjectId(parsed._id);
        }

        query = parsed;
      } catch (err) {
        return Response.json({ error: 'Invalid JSON in query parameter `q`.' }, { status: 400 });
      }
    }


    await connectToMongo();
    const db = mongoose.connection.db;
    const col = db.collection(collection);
    const docs = await col.find(query).toArray();

    return Response.json({ count: docs.length, data: docs }, { status: 200 });
  } catch (err) {
    console.error('Mongo GET error:', err);
    return Response.json({ error: 'Database operation failed', detail: err.message }, { status: 500 });
  }
}

export async function PUT(request) {
  try {
    const { searchParams } = new URL(request.url);
    const collection = searchParams.get('collection');
    if (!collection) {
      return Response.json({ error: 'Missing `collection` in query params.' }, { status: 400 });
    }

    const { filter, update } = await request.json();
    if (!filter || !update) {
      return Response.json({ error: 'Missing `filter` or `update` in body.' }, { status: 400 });
    }

    if (filter._id) {
      try {
        filter._id = new ObjectId(filter._id);
      } catch {
        return Response.json({ error: 'Invalid ObjectId format in `filter._id`.' }, { status: 400 });
      }
    }

    await connectToMongo();
    const db = mongoose.connection.db;
    const col = db.collection(collection);
    const result = await col.updateMany(filter, { $set: update });

    return Response.json({
      success: true,
      matchedCount: result.matchedCount,
      modifiedCount: result.modifiedCount,
    }, { status: 200 });
  } catch (err) {
    console.error('Mongo PUT error:', err);
    return Response.json({ error: 'Database operation failed', detail: err.message }, { status: 500 });
  }
}

export async function DELETE(request) {
  try {
    const { searchParams } = new URL(request.url);
    const collection = searchParams.get('collection');
    if (!collection) {
      return Response.json({ error: 'Missing `collection` in query params.' }, { status: 400 });
    }

    const { filter } = await request.json();
    if (!filter || typeof filter !== 'object') {
      return Response.json({ error: 'Missing or invalid `filter` in body.' }, { status: 400 });
    }

    if (filter._id) {
      try {
        filter._id = new ObjectId(filter._id);
      } catch {
        return Response.json({ error: 'Invalid ObjectId format in `filter._id`.' }, { status: 400 });
      }
    }

    await connectToMongo();
    const db = mongoose.connection.db;
    const col = db.collection(collection);
    const result = await col.deleteOne(filter);

    return Response.json({ success: true, deletedCount: result.deletedCount }, { status: 200 });
  } catch (err) {
    console.error('Mongo DELETE error:', err);
    return Response.json({ error: 'Database operation failed', detail: err.message }, { status: 500 });
  }
}
