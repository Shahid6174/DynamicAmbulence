import mongoose from 'mongoose';

const MONGODB_URI = process.env.MONGODB_URI;

const connect = async () => {
  if (mongoose.connection.readyState === 1) return;
  await mongoose.connect(MONGODB_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
  });
};

export default async function handler(req, res) {
  const { collection } = req.query;

  if (!collection) {
    return res.status(400).json({ error: 'Missing `collection` in query' });
  }

  await connect();
  const db = mongoose.connection.db;
  const col = db.collection(collection);

  try {
    switch (req.method) {
      //  CREATE
      case 'POST': {
        const { data } = req.body;
        if (!data) return res.status(400).json({ error: 'Missing `data` in body' });
        const result = await col.insertOne(data);
        return res.status(201).json({ success: true, insertedId: result.insertedId });
      }

      //  READ
      case 'GET': {
        const query = req.query.q ? JSON.parse(req.query.q) : {};
        const docs = await col.find(query).toArray();
        return res.status(200).json(docs);
      }

      //  UPDATE
      case 'PUT': {
        const { filter, update } = req.body;
        if (!filter || !update) {
          return res.status(400).json({ error: 'Missing `filter` or `update` in body' });
        }
        const result = await col.updateMany(filter, { $set: update });
        return res.status(200).json({ success: true, modifiedCount: result.modifiedCount });
      }

      //  DELETE
      case 'DELETE': {
        const { filter } = req.body;
        if (!filter) {
          return res.status(400).json({ error: 'Missing `filter` in body' });
        }
        const result = await col.deleteMany(filter);
        return res.status(200).json({ success: true, deletedCount: result.deletedCount });
      }

      default:
        return res.status(405).end('Method Not Allowed');
    }
  } catch (err) {
    console.error('Operation failed:', err);
    return res.status(500).json({ error: 'MongoDB operation failed', detail: err.message });
  }
}
